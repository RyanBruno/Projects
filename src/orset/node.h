/* Represents some logic to bring together
 * all modules into a full functioning mode.
 * Its job is to handle incoming merge
 * requests and occasionally send merge
 * request to peers.
 */
#ifndef NODE_H
#define NODE_H

#include <semaphore.h>
#include "config.h"
#include "ospc/ospc.h"

/* Merges a remote set with node's local
 * set.
 */
uint64_t merge_request(struct orset* rmt_os);

/* The client thread occasionally sends
 * merge requests to other peers. It makes
 * sure it does not try to merge with
 * itself based on node_id.
 */
void* client_thread_fn(void* v);

/* Inits global OrSet, semaphore and RPC
 * framework.
 */
int node_init(node_t node_id, unsigned long prognum);

/* Used to build the baseline network
 * information about a single peer.
 */
struct peer_node {
    unsigned short peer_id;     // The node's peer id (right now unused).
    char* peer_host;            // The node's host name (passed in to clnt_create()).
    unsigned long peer_prognum; // The RPC prognum (see man rpc(3)).
};

/* Global orset and ospc_context */
extern struct orset os;
extern struct ospc_context oc;

/* A global lock to make edits and reads to
 * the OrSet.
 */
extern sem_t os_sem;

#endif /* NODE_H */
