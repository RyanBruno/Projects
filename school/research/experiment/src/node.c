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
#include "utils.h"
#include "../config.h"

/* Global Node variables */
int node_id;
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

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: Invalid arguments.\n");

        /* Send an reply */
        svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL);
        return;
    }

    /* Merge */
    sem_wait(&os_sem);
    ospc_merge(&oc, &rmt_os);
    if (node_id == 0) print_set(&os);
    sem_post(&os_sem);

    /* Cleanup */
    /* TODO for some reason this function spins for along time */
    /*if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: free\n");*/

        /* Send an reply */
        /*svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL);*/

        /* Exit */
        //exit(-1);
        //return;
    //}

    /* Send a reply */
    svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL);
}

/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int register_procedure(long int prognum)
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
                rpc_merge_request, IPPROTO_TCP))
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
    int n = 0;

    /* Grace period */
    sleep(GRACE_PERIOD);

    for (;;) {
        struct timeval to;
        CLIENT* client;

        /* Setup timeout */
        to.tv_sec = MERGE_TIMEOUT;

        /* Wrap and skip node_id */
        if (n == node_id) n++;
        if (n >= peers_len) n = 0;
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
            n++;
            sleep(5);
            continue;
        }

        /* Actually call the procedure. Right
         * now there is only 1 procedure so
         * the second parameter is unused.
         */
        clnt_call(client, 1,
                 /* Params */
                 (xdrproc_t) xdr_orset, oc.oc_orset,
                 /* Response */
                 (xdrproc_t) xdr_void, NULL,
                 to);

        /* Wait a little */
        n++;
        sleep(MERGE_PERIOD);
    }
}

int main(int argc, char* argv[])
{
    int n;

    pthread_t thread;

    /* Check args */
    if (argc < 2) {
        printf("USAGE: ./node NODEID\n");
        return -1;
    }

    /* Parse NODEID */
    node_id = strtol(argv[1], NULL, 10);

    if (node_id >= peers_len) {
        /* TODO */
    }

    /* Create our OrSet */
    orset_create(&os, node_id);
    ospc_wrap(&os, &oc);

    /* Create a semaphore so our two threads
     * do not step on each other.
     */
    if (sem_init(&os_sem, 0, 1)) {
        printf("sem_init():\n");
        exit(-1);
    }

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
