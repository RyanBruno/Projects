/* Represents some logic to bring together
 * all modules into a full functioning mode.
 * Its job is to handle incoming merge
 * requests and occasionally send merge
 * request to peers.
 */
#ifndef NODE_H
#define NODE_H

#include <semaphore.h>
#include "ospc/ospc.h"
#include "xdr_orset/xdr_orset.h"

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

extern int node_id;

#endif
