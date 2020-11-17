
#include <event2/buffer.h>

/* net's own vectorized io struct.
 */
struct net_iov_t {
    char* iov_base;
    size_t iov_len;
};

/* Passed to net_listen to setup callbacks.
 */
struct net_context_t {
    // Called every time some data is read.
    size_t(*nc_read)(struct net_iov_t*, ssize_t, void*);
    // Called when a socket has been accepted.
    // It's return value is passed into nc_read.
    void*(*nc_accept)();
};

/* Open and register a socket to listen on
 * a port. Callbacks are setup through
 * net_context_t.
 */
void net_listen(int port, struct net_context_t* nc);

/* This function never returns. It listens
 * for events from sockets and triggers
 * callbacks.
 */
void net_dispatch();
