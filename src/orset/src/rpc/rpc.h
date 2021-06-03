#include <rpc/rpc.h>

/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int rpc_init(unsigned long prognum);

/* Sends our OrSet to the 'target_peer'
 * putting the result in 'res'.
 */
enum clnt_stat rpc_merge_make_request(int target_peer, uint64_t* res);
