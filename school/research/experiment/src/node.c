/* Represents some logic to bring together
 * all modules into a full functioning mode.
 * Its job is to handle incoming merge
 * requests and occasionally send merge
 * request to peers.
 */
#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "node.h"
#include "xdr_orset/xdr_orset.h"
#include "utils.h"
#include "../config.h"
#include "osec/osec.h"

/* Global Node variables */
int node_id;
uint64_t last_stable_item = 0;
struct orset os;
struct ospc_context oc;
sem_t os_sem;


/* Is called by the RPC library when ever a
 * merge request comes in. All it does so
 * far is parse with xdr_orset, merge the
 * remote set with the local one using the
 * latest merge algorithm, free the remote
 * set.
 */
void rpc_merge_request(struct svc_req *req, SVCXPRT *xprt)
{
    struct orset rmt_os;
    uint64_t latest_item;

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: Invalid arguments.\n");

        /* Send an reply */
        latest_item = 0;
        svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &latest_item);

        return;
    }

    /* Merge */
    sem_wait(&os_sem);
    latest_item = ospc_merge(&oc, &rmt_os);
    print_set_stats(&os, node_id);
    sem_post(&os_sem);

    /* Cleanup */
    if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: free\n");

        /* Exit */
        exit(-1);
    }

    /* Send a reply */
    svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &latest_item);
}

/* This is sent from a node when it has
 * received confirmation from every node
 * that a tombstone has been received. The
 * algorithm here is to update
 * oc_latest_key_map then collect all
 * tombstones below.
 */
void rpc_eager_request(struct svc_req *req, SVCXPRT* xprt)
{
    uint64_t latest_item;

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_uint64_t, &latest_item)) {
        printf("rpc_merge_request: Invalid arguments.\n");

        svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL);
        return;
    }

    /* Merge */
    sem_wait(&os_sem);
    osec_eager_collect(&oc, latest_item);
    sem_post(&os_sem);

    /* Send a reply */
    svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL);
}

/* A simple router that calls the right
 * function based on procedure number.
 */
void rpc_request(struct svc_req *req, SVCXPRT *xprt)
{
    switch (req->rq_proc) {
    case 1:
        rpc_merge_request(req, xprt);
        break;
    case 2:
        rpc_eager_request(req, xprt);
        break;
    default:
        printf("Invalid prognum\n");
        break;
    }
}


/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int register_procedure(unsigned long prognum)
{
    int rc;
    SVCXPRT* xprt;

    /* Create a RPC service socket. */
    if ((xprt = svctcp_create(RPC_ANYSOCK, 0, 0)) == NULL) {
        printf("register_procedure: Could not create service.\n");
        return -1;
    }

    /* Un-register any previous registrations
     * on our prognum.
     */
    pmap_unset(prognum, VERSION_NUMBER);

    /* Register rpc_merge_request procedure */
    if (!svc_register(xprt, prognum, VERSION_NUMBER,
                rpc_request, IPPROTO_TCP))
    {
        printf("register_procedure: Could not register.\n");
        return -1;
    }

    return 0;
}

/* The client thread occasionally sends
 * merge requests to other peers. It makes
 * sure it does not try to merge with
 * itself based on node_id.
 */
void* client_thread_fn(void* v)
{
    int n = node_id + 1;

    for (;;) {
        enum clnt_stat stat;
        struct timeval to;
        CLIENT* client;
        uint64_t latest_item;
        uint64_t stable_item;

        /* Wait a little */
        n++;
        sleep(MERGE_RATE);

        /* Setup timeout */
        to.tv_sec = MERGE_TIMEOUT;
        to.tv_usec = 0;

        /* Wrap and skip node_id */
        if (n == node_id) n++;
        if (n >= PEERS_LEN) n = 0;
        if (n == node_id) n++;

        /* Create a client to our peer */
        client = clnt_create(peers[n].peer_host,
                             peers[n].peer_prognum,
                             VERSION_NUMBER,
                             "tcp");

        /* If client creation fails our peer
         * is not registered with rpcbind(1)
         * or otherwise unavailable.
         */
        if (client == NULL) {
            printf("client_thread_fn: clnt_create(3) has failed!\n");
            continue;
        }

        /* Actually call the procedure. We
         * want to merge with our peer so we
         * call procedure number 1.
         */
        stat = clnt_call(client, 1,
                 /* Params */
                 (xdrproc_t) xdr_orset, oc.oc_orset,
                 /* Response */
                 (xdrproc_t) xdr_uint64_t, &latest_item,
                 to);

        if (stat != RPC_SUCCESS) {
            printf("client_thread_fn(): clnt_call(%d)\n", stat);
            continue;
        }

        /* Close the connection to our peer */
        clnt_destroy(client);

        /* Runs the OrSet garbage collector */
        sem_wait(&os_sem);

        /* Updates the latest_key_map for
         * this current peer. If not updated
         * continue.
         */
        if (!ospc_touch(&oc, n, latest_item)) {
            sem_post(&os_sem);
            continue;
        }

        /* If updated try to collect some
         * tombstones and get the greatest
         * stable item (or least 
         */
        stable_item = ospc_collect(&oc);

        sem_post(&os_sem);

        if (!stable_item ||
            last_stable_item + EAGER_RATE > stable_item)
            continue;

        for (int j = 0; j < PEERS_LEN; j++) {

            /* Do not send to self */
            if (j == node_id) continue;

            /* Setup timeout */
            to.tv_sec = MERGE_TIMEOUT;
            to.tv_usec = 0;

            /* Create a client to our peer */
            client = clnt_create(peers[j].peer_host,
                                 peers[j].peer_prognum,
                                 VERSION_NUMBER,
                                 "tcp");

            /* If client creation fails our peer
             * is not registered with rpcbind(1)
             * or otherwise unavailable.
             */
            if (client == NULL) {
                printf("client_thread_fn: clnt_create(3) has failed!\n");
                continue;
            }

            /* Tells the peer it should eager
             * collect.
             */
            stat = clnt_call(client, 2,
                     /* Params */
                     (xdrproc_t) xdr_uint64_t, &stable_item,
                     /* Response */
                     (xdrproc_t) xdr_void, NULL,
                     to);

            if (stat != RPC_SUCCESS) {
                printf("client_thread_fn(): clnt_call(%d)\n", stat);
                continue;
            }

            /* Close the connection to our peer */
            clnt_destroy(client);
        }
        last_stable_item = stable_item;
    }
}

/* This function is great for stating a
 * node without using the main() function
 * below. It creates the global OrSet and
 * semaphore.
 */
int node_init()
{
    /* Create our OrSet */
    orset_create(&os, node_id);
    ospc_wrap(&os, &oc);

    /* Create a semaphore so our two threads
     * do not step on each other.
     */
    if (sem_init(&os_sem, 0, 1)) {
        printf("sem_init():\n");
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int n;

    pthread_t thread;

    utils_start();

    /* Check args */
    if (argc < 2) {
        printf("USAGE: ./node NODEID MERGE_RATE OPERATION_RATE ADD_TO_REM_RATIO PEERS_LEN DURATION EAGER_RATE\n");
        return -1;
    }

    /* Parse NODEID */
    node_id = strtol(argv[1], NULL, 10);

    /* Parse command line params */
    if (argc > 2) {
        MERGE_RATE = strtol(argv[2], NULL, 10);
        if (argc > 3) {
            OPERATION_RATE = strtol(argv[3], NULL, 10);
            if (argc > 4) {
                ADD_TO_REM_RATIO = strtol(argv[4], NULL, 10);
                if (argc > 5) {
                    PEERS_LEN = strtol(argv[5], NULL, 10);
                    if (argc > 6) {
                        DURATION = strtol(argv[6], NULL, 10);
                        if (argc > 6) {
                            EAGER_RATE = strtol(argv[7], NULL, 10);
                        }
                    }
                }
            }
        }
    }

    if (node_id >= PEERS_LEN) {
        printf("main(): NODEID is our of range\n");
        return -1;
    }

    /* Create our node's OrSet */
    if (node_init() < 0)
        return -1;

    /* Create a thread to send merge messages */
    if (pthread_create(&thread,
                NULL,
                client_thread_fn,
                NULL))
    {
        printf("pthread_create():\n");
        exit(-1);
    }

    /* Creates all the threads in 'threads' */
    for (int i = (sizeof(threads) / sizeof(threads[0]));
         i > 0; i--)
    {
        /* Create a thread to adding and removing items */
        if (pthread_create(&thread,
                    NULL,
                    threads[i - 1],
                    NULL))
        {
            printf("pthread_create():\n");
            exit(-1);
        }
    }

    /* Register our service */
    if (register_procedure(peers[node_id].peer_prognum)) {
        printf("register_procedure():\n");
        exit(-1);
    }

    /* This function never returns. It
     * waits for requests then runs
     * rpc_merge_request.
     */
    svc_run();

    return -1;
}
