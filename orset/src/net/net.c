
#include <event2/event.h>

struct event_base *net_event_base = NULL;

void net_accept_err_cb(struct evconnlistener *listener, void *ctx)
{
    printf("net_accept_err_cb(): %s\n",
            evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));

    evconnlistener_free(listener)

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

void net_read_cb(struct bufferevent *bev, void *arg)
{
    // A read has taken place.
}

void net_accept_cb(struct evconnlistener *listener, evutil_socket_t sock,
        struct sockaddr *addr, int len, void *ptr)
{
    struct timeval five_seconds = {5,0};
    struct bufferevent *bev;

    bev = bufferevent_socket_new(net_event_base,
                                 sock,
                                 BEV_EVENT_READING | BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev,
                      net_read_cb,
                      net_write_cb,
                      net_event_cb,
                      NULL);

    event_add(ev1, &five_seconds);
}

void net_listen(int port)
{
    struct sockaddr_in addr;
    struct evconnlistener *li;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("0.0.0.0", &addr.sin_addr.s_addr)

    if (net_event_base == NULL)
        net_event_base = event_base_new(void);

    li = evconnlistener_new_bind(net_event_base,
                                 net_accept_cb,
                                 NULL, // Args
                                 LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                 -1, // Back Log
                                 addr, sizeof(addr));

    evconnlistener_set_error_cb(li, net_accept_err_cb);

}

void netapi_setup()
{
    if (net_event_base == NULL)
        net_event_base = event_base_new(void);

    event_base_dispatch(net_event_base);
}

