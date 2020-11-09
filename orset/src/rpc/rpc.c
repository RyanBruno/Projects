#include "rpc.h"

#include "../node.h"
#include "../xdr_orset/xdr_orset.h"
#include "../../config.h"

/* From config.h */
extern struct peer_node* peers;

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
    uint64_t rply;

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        fprintf(stderr, "rpc_merge_request: Invalid arguments.\n");

        /* Send an reply */
        rply = 0;
        svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &rply);

        return;
    }

    /* Pass the rmt_os to node and let node
     * handle the merge.
     */
    rply = merge_request(&rmt_os);

    /* Cleanup */
    if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        fprintf(stderr, "rpc_merge_request: free\n");

        /* Exit */
        exit(-1);
    }

    /* Send a reply */
    svc_sendreply(xprt, (xdrproc_t) xdr_uint64_t, &rply);
}

/* Sends our entire OrSet to the target
 * peer. Puts the response in res.
 */
enum clnt_stat rpc_merge_make_request(int target_peer, uint64_t* res)
{
    enum clnt_stat stat;
    struct timeval to;
    CLIENT* client;

    /* Setup timeout */
    to.tv_sec = MERGE_TIMEOUT;
    to.tv_usec = 0;

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
        return RPC_FAILED;
    }

    /* Actually call the procedure. We
     * want to merge with our peer so we
     * call procedure number 1.
     */
    stat = clnt_call(client, 1,
             /* Params */
             (xdrproc_t) xdr_orset, oc.oc_orset,
             /* Response */
             (xdrproc_t) xdr_uint64_t, res,
             to);

    /* Close the connection to our peer */
    clnt_destroy(client);

    return stat;

}

/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int rpc_init(unsigned long prognum)
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
