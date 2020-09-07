#include "src/node.h"
#include "src/demo.h"

/* We achieve "plugin-like" modularity by
 * adding thread functions to this array.
 * Theses threads are creates on startup.
 */
void *(*threads[]) (void*) = {
    demo_thread_fn,
};

/* RPC configuration points */

/* Passed directly to svc_register and
 * clnt_create.
 */
#define VERSION_NUMBER 1

/* Client thread configuration points */

/* The amount of time (in seconds) to wait
 * before we start trying to merge with
 * peers.
 */
#define GRACE_PERIOD 5

/* The amount of time between merge
 * requests.
 */
#define MERGE_PERIOD 5

/* The timeout for a single merge request */
#define MERGE_TIMEOUT 20

/* Describes the network of peers. This
 * array contains the current node_id.
 * The client thread skips it.
 */
struct peer_node peers[10] = {
    { .peer_id = 0, .peer_host = "127.0.0.1", .peer_prognum = 100600L },
    { .peer_id = 0, .peer_host = "127.0.0.1", .peer_prognum = 100601L },
    { .peer_id = 0, .peer_host = "127.0.0.1", .peer_prognum = 100602L },
    { .peer_id = 0, .peer_host = "127.0.0.1", .peer_prognum = 100603L },
};
int peers_len = 2;

