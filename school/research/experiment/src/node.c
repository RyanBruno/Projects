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

/* From config.h */
extern time_t MERGE_RATE;
extern unsigned int EAGER_RATE;
extern int PEERS_LEN;
extern struct peer_node peers[20];

/* Todo fix */
#define VERSION_NUMBER 1
#define MERGE_TIMEOUT 20

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
    uint64_t latest_item;

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        fprintf(stderr, "rpc_merge_request: Invalid arguments.\n");

        /* Send an reply */
        latest_item = 0;
        svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &latest_item);

        return;
    }

    /* Merge */
    sem_wait(&os_sem);
    latest_item = ospc_merge(&oc, &rmt_os);
    sem_post(&os_sem);

    /* Cleanup */
    if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        fprintf(stderr, "rpc_merge_request: free\n");

        /* Exit */
        exit(-1);
    }

    /* Send a reply */
    svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &latest_item);
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
        fprintf(stderr, "register_procedure: Could not create service.\n");
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
        fprintf(stderr, "register_procedure: Could not register.\n");
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
    for (;;) {
        enum clnt_stat stat;
        struct timeval to;
        CLIENT* client;
        uint64_t latest_item;
        int target_peer;

        /* Wait a little */
        sleep(MERGE_RATE);

        /* Setup timeout */
        to.tv_sec = MERGE_TIMEOUT;
        to.tv_usec = 0;

        /* Pick a peer to merge with. */
        target_peer = rand() % (PEERS_LEN - 1);
        if (target_peer >= node_id) target_peer++;

        /* Create a client to our peer */
        client = clnt_create(peers[target_peer].peer_host,
                             peers[target_peer].peer_prognum,
                             VERSION_NUMBER,
                             "tcp");

        /* If client creation fails our peer
         * is not registered with rpcbind(1)
         * or otherwise unavailable.
         */
        if (client == NULL) {
            fprintf(stderr, "client_thread_fn: clnt_create(3) has failed!\n");
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
            fprintf(stderr, "client_thread_fn(): clnt_call(%d)\n", stat);
            continue;
        }

        /* Close the connection to our peer */
        clnt_destroy(client);
        continue;

        /* Runs the OrSet garbage collector */
        sem_wait(&os_sem);

        /* Updates the latest_key_map for
         * this current peer. If not updated
         * continue.
         */
        if (ospc_touch(&oc, target_peer, latest_item)) {
            /* If updated try to collect some
             * tombstones.
             */
            ospc_collect(&oc);
        }

        sem_post(&os_sem);
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
        fprintf(stderr, "sem_init():\n");
        return -1;
    }

    return 0;
}
