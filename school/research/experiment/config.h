#include "src/node.h"
#include "src/demo.h"

/* We achieve "plugin-like" modularity by
 * adding thread functions to this array.
 * Theses threads are creates on startup.
 */
void *(*threads[]) (void*) = {
    demo_thread_fn,
};

/* General Configuration */

/* This defines how big the node id should
 * be. Your only reasonable choices are
 * uint16_t or uint8_t.
 * uint8_t => max value = 256
 * uint16_t => max value = 65,536
 * TODO: Unused.
 */
#define node_t uint8_t

/* RPC configuration points */

/* Passed directly to svc_register and
 * clnt_create.
 */
unsigned long VERSION_NUMBER = 1;
//#define VERSION_NUMBER 1

/* Client thread configuration points */

/* The amount of time between merge
 * requests.
 */
unsigned int MERGE_RATE = 1;
//#define MERGE_PERIOD 1

/* The timeout for a single merge request */
time_t MERGE_TIMEOUT = 20;
//#define MERGE_TIMEOUT 20

/* DEMO configuration points */

/* The ratio of add to removes for each demo
 * operation.
 */
unsigned int ADD_TO_REM_RATIO = 5;

/* The amount of seconds between each demo
 * operation. May also be represented as
 * seconds per operation.
 */
time_t OPERATION_RATE = 1;

/* Eager Collection */

/* How many collected local nodes before
 * initiating eager collection.
 */
#define EAGER_COLLECTION_TRIGGER 10

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
int peers_len = 4;

