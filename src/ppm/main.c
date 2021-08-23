#define ptr_move(x) x

/* buf */
#include "buf.h"

/* http */
#define T1 buf
#define T1_PREFIX buf_
#include "http.h"

/* work */
#define T1 net
#define T1_PREFIX net_
#include "work.h"

/* epoll_event */
#define T1 work
#define T1_PREFIX work_
#include "epoll_event.h"

/* linux_net */
#define T1 work
#define T1_PREFIX work_
#include "linux_net.h"

/* net */
#define T1 linux_net
#define T1_PREFIX linux_net_
#define T2 struct epoll_event
#define T2_PREFIX epoll_event_
#define T3 work
#define T3_PREFIX work_
#define T4 buf
#define T4_PREFIX buf_
#define T5 http
#define T5_PREFIX http_
#include "net.h"

int main()
{
    net_listen_inet(8889);
}
