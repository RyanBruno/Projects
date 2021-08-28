#include <sys/socket.h>
#include <arpa/inet.h>
#define ptr_move(x) x

/* buf */
#include "buf.h"

/* router */
#define T1 buf
#define T1_PREFIX buf_
#include "router.h"

/* http */
#define T1 buf
#define T1_PREFIX buf_
#define T2 struct router
#define T2_PREFIX router_
#include "http.h"

/* work */
#define T1 struct net
#define T1_PREFIX net_
#include "work.h"

/* epoll_event */
#define T1 struct work
#define T1_PREFIX work_
#include "epoll_event.h"

/* linux_net */
#define T1 struct work
#define T1_PREFIX work_
#include "linux_net.h"

/* net */
#define T1 linux_net
#define T1_PREFIX linux_net_
#define T2 struct epoll_event
#define T2_PREFIX epoll_event_
#define T3 struct work
#define T3_PREFIX work_
#define T4 buf
#define T4_PREFIX buf_
#define T5 http
#define T5_PREFIX http_
#include "net.h"

int main(int argc, char* argv[])
{
    assert(argc > 1);
    net_listen_inet((int) strtol(argv[1], NULL, 10));
}
