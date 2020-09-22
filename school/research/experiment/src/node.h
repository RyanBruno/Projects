/* Represents some logic to bring together
 * all modules into a full functioning mode.
 * Its job is to handle incoming merge
 * requests and occasionally send merge
 * request to peers.
 */
#ifndef NODE_H
#define NODE_H

#include <semaphore.h>

/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int register_procedure(unsigned long prognum);

/* The client thread occasionally sends
 * merge requests to other peers. It makes
 * sure it does not try to merge with
 * itself based on node_id.
 */
void* client_thread_fn(void* v);

/* This function is great for stating a
 * node without using the main() function
 * below. It creates the global OrSet and
 * semaphore.
 */
int node_init();

/* Used to build the baseline network
 * information about a single peer.
 */
struct peer_node {
    unsigned short peer_id;     // The node's peer id (right now unused).
    char* peer_host;            // The node's host name (passed in to clnt_create()).
    unsigned long peer_prognum; // The RPC prognum (see man rpc(3)).
};

#include "ospc/ospc.h"

/* Global orset and ospc_context */
extern struct orset os;
extern struct ospc_context oc;

/* A global lock to make edits and reads to
 * the OrSet.
 */
extern sem_t os_sem;

#endif
