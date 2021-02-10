#include "net.h"

#include <stdlib.h>
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

struct event_base *net_event_base = NULL;

/* Represents the context needed for a
 * socket. ns_nc points to the net_context_t
 * containing the callbacks and ns_v holds
 * a pointer that means something to those
 * callbacks about this specific socket.
 */
struct net_sock_context_t {
    struct net_context_t* ns_nc;
    void* ns_v;
};

void net_accept_err_cb(struct evconnlistener *listener, void *ctx)
{
    printf("net_accept_err_cb(): %s\n",
            evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));

    evconnlistener_free(listener);

    exit(-1);
}

void net_event_cb(struct bufferevent *bev, short what, void *arg)
{
        // An error occurred during a bufferevent operation. For more information on what the error was, call EVUTIL_SOCKET_ERROR().
    if (what & BEV_EVENT_ERROR) {
        printf("net_event_cb(): %s\n",
                evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
        /* Close socket */
        bufferevent_free(bev);
    } else
        // A timeout expired on the bufferevent.
    if (what & BEV_EVENT_TIMEOUT) {
    } else

        // We got an end-of-file indication on the bufferevent.
    if (what & BEV_EVENT_EOF) {
        bufferevent_free(bev);
    } else

    if (what & BEV_EVENT_CONNECTED) {
    }
        // We finished a requested connection on the bufferevent.
}

void net_write_cb(struct bufferevent *bev, void *arg)
{
    // All data has been written.
}

/* A read has taken place. We now push it
 * to nc_read callback via vectorized io.
 * This may call nc_read multiple times
 * if libevent decides it does not want
 * to give us all its data.
 */
void net_read_cb(struct bufferevent *bev, void *arg)
{
    struct evbuffer* buf = bufferevent_get_input(bev);
    struct net_sock_context_t* ns = (struct net_sock_context_t*) arg;
    struct evbuffer_iovec* iov;
    int n_vec;
    size_t len;

    /* How many iovecs are there? */
    while ((n_vec = evbuffer_peek(buf, 4096, NULL, NULL, 0)) > 0) {
        /* alloca that much memory. */
        iov = alloca(sizeof(struct evbuffer_iovec) * n_vec);
        /* Fill them up. */
        n_vec = evbuffer_peek(buf, 4096, NULL, iov, n_vec);
        /* Push to nc_read callback. */
        len = ns->ns_nc->nc_read((struct net_iov_t*) iov, n_vec, ns->ns_v);
        /* Remove read data from the buffer. */
        evbuffer_drain(buf, len);
    }
}

/* A new socket has been accepted. This
 * function setups the socket to be read
 * on and creates a net_sock_conext_t for
 * it. It also calls nc_accept and sets
 * ns_v to its result.
 */
void net_accept_cb(struct evconnlistener *listener, evutil_socket_t sock,
        struct sockaddr *addr, int len, void *ptr)
{
    /* TODO timeouts */
    struct bufferevent *bev;
    struct net_context_t* nc = (struct net_context_t*) ptr;
    struct net_sock_context_t* ns;
    void* ctx;

    /* Create a bufferevent. */
    bev = bufferevent_socket_new(net_event_base,
                                 sock,
                                 BEV_OPT_CLOSE_ON_FREE);

    /* Create a net_sock_conext_t. */
    ns = malloc(sizeof(struct net_sock_context_t));
    ns->ns_nc = nc;
    /* Gives this socket some context through
     * nc_accept. Passed to nc_read for this
     * socket. */
    ns->ns_v = nc->nc_accept();

    /* Set callbacks. */
    bufferevent_setcb(bev,
                      net_read_cb,
                      net_write_cb,
                      net_event_cb,
                      ns);

    /* Enable. */
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

/* Open and register a socket to listen on
 * a port. Callbacks are setup through
 * net_context_t.
 */
void net_listen(int port, struct net_context_t* nc)
{
    struct sockaddr_in addr;
    struct evconnlistener *li;

    /* Setup addr */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("0.0.0.0", &addr.sin_addr);
    //.s_addr

    /* If not created already... */
    if (net_event_base == NULL)
        /* Create event_base */
        net_event_base = event_base_new();

    /* Setup listener. */
    li = evconnlistener_new_bind(net_event_base,
                                 net_accept_cb, // Callback.
                                 nc, // Args.
                                 LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                 -1, // Back Log
                                 (const struct sockaddr*) &addr, sizeof(addr));

    /* Set error callback. */
    evconnlistener_set_error_cb(li, net_accept_err_cb);
}

/* This function never returns. It listens
 * for events from sockets and triggers
 * callbacks.
 */
void net_dispatch()
{
    /* If not created already... */
    if (net_event_base == NULL)
        /* Create event_base */
        net_event_base = event_base_new();

    /* Never returns. Listens for events
     * and triggeres callbacks.
     */
    event_base_dispatch(net_event_base);
}

