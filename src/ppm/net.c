#include <setjmp.h>
#include <ucontext.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>

/* Work */
typedef struct {
    void (*fn)(void*, void* v);
    void* ctx;
} work;
void work_move(work* cur, work* other)
{
    memcpy(other, cur, sizeof(work));
}
void work_exec(work* w, void* v)
{
    w->fn(w->ctx, v);
}
/* End: Work */
/* Epoll Event */
#define T1 work
#define T1_PREFIX work_

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
void epoll_event_exec(struct epoll_event* ev, void* v)
{
    T1_CCAT(exec)(ev->data.ptr, ev);
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
/* End: Epoll Event */
/* Linux Net */
#define T1 work
#define T1_PREFIX work_

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
typedef struct {
    int epoll;
} linux_net;

/* TODO standarize constructors */
void linux_net_construct(linux_net* lux)
{
    lux->epoll = epoll_create1(0);

    assert(lux->epoll > -1 || errno == EINVAL);
    assert(lux->epoll > -1 || errno == EMFILE);
    assert(lux->epoll > -1 || errno == ENFILE);
    assert(lux->epoll > -1 || errno == ENOMEM);
    assert(lux->epoll > -1);
}
void linux_net_move(linux_net* cur, linux_net* other)
{
    other->epoll = cur->epoll;
    cur->epoll = -1;
}
void linux_net_copy(linux_net* cur, linux_net* other)
{
    // TODO refcounting
    other->epoll = cur->epoll;
}
/* Maybe rename: ready? */
int linux_net_select(linux_net* lux, struct epoll_event* e, int c, int timeout)
{
    int n;

again:
    printf("Listening....\n");
    n = epoll_wait(lux->epoll, e, c, -1);

    if (n == 0) {
        /* Timeout */
        printf("Timeout\n");
        goto again;
    }

    if (n < 0) {
        assert(errno != EBADF);
        assert(errno != EFAULT);
        assert(errno != EINVAL);
        goto again; // EINTR
    }

    return n;
}
void linux_net_insert(linux_net* lux, int fd, T1 wk)
{
    int rc;
    struct epoll_event ev;
    T1* mem;

    mem = malloc(sizeof(T1));
    T1_CCAT(move)(&wk, mem);

    ev.events = EPOLLIN | EPOLLPRI;
                //EPOLLOUT
                //EPOLLET
    ev.data.ptr = mem;

    rc = epoll_ctl(lux->epoll, EPOLL_CTL_ADD, fd, &ev);
    assert(rc > -1 || errno == EBADF);
    assert(rc > -1 || errno == EEXIST);
    assert(rc > -1 || errno == EINVAL);
    assert(rc > -1 || errno == ELOOP);
    assert(rc > -1 || errno == ENOENT);
    assert(rc > -1 || errno == ENOMEM);
    assert(rc > -1 || errno == ENOSPC);
    assert(rc > -1 || errno == EPERM);
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
/* End: Linux Net */
/* Generic */
#define T1 linux_net
#define T1_PREFIX linux_net_
#define T2 struct epoll_event
#define T2_PREFIX epoll_event_

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
#define T2_CCAT(x) CCAT2(T2_PREFIX, x)
void net_exec(T1* lux)
{
    int n;
    T2 e[10];

    for (;;) {
        n = T1_CCAT(select)(lux, (T2*) &e, 10, 100000);

        for (int i = 0; i < n; i++) {
            T2_CCAT(exec)(
                    &e[i],
                    NULL);
        }
    }
}
#undef T2_CCAT
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T2_PREFIX
#undef T1_PREFIX
#undef T2
#undef T1
/* End: Generic */
/* Generic */
#define T1 linux_net
#define T1_PREFIX linux_net_
#define T2 struct epoll_event
#define T2_PREFIX epoll_event_

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
#define T2_CCAT(x) CCAT2(T2_PREFIX, x)
typedef struct {
    T1 lux;
    int fd;
    struct sockaddr_in addr;
} net;
void net_move(net* cur, net* other)
{
    T1_CCAT(move)(&cur->lux, &other->lux);
    other->fd = cur->fd;
    memcpy(&cur->addr, &other->addr, sizeof(struct sockaddr_in));
}
void net_read(void* ctx, void *v)
{
    net* n = (net*) ctx;
    T2* ev = (T2*) v;

    if (ev->events & EPOLLIN || ev->events & EPOLLPRI) {
        char b[128];
        ssize_t s;

        s = read(n->fd, b, 127);

        if (s == 0) {
            printf("EOF\n");
            exit(0);
        }

        if (s < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        assert(s > -1);

        b[s] = '\0';
        printf("Read %d: %s\n", s, b);
        return;
    }
    assert(!(ev->events & EPOLLOUT));
    assert(!(ev->events & EPOLLERR));
    assert(!(ev->events & EPOLLHUP));
    assert(0);
}
void net_accept(void *ctx, void* v)
{
    net* n = (net*) ctx;
    T2* ev = (T2*) v;

    if (ev->events & EPOLLIN || ev->events & EPOLLPRI) {
        net nn;
        work wk;
        socklen_t addr_len = sizeof(n->addr);
        int rc;
        int flags;

        /* Setup new net struct */
        T1_CCAT(copy)(n->lux, nn.lux);

        nn.fd = accept(n->fd, (struct sockaddr*) &nn.addr, &addr_len);

        if (nn.fd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        assert(nn.fd > -1 || errno == EBADF);
        assert(nn.fd > -1 || errno == ECONNABORTED);
        assert(nn.fd > -1 || errno == EINTR);
        assert(nn.fd > -1 || errno == EINVAL);
        assert(nn.fd > -1 || errno == EMFILE);
        assert(nn.fd > -1 || errno == ENFILE);
        assert(nn.fd > -1 || errno == ENOBUFS);
        assert(nn.fd > -1 || errno == ENOMEM);
        assert(nn.fd > -1 || errno == ENOTSOCK);
        assert(nn.fd > -1 || errno == EOPNOTSUPP);
        assert(nn.fd > -1 || errno == EPROTO);
        assert(nn.fd > -1);

        /* Set socket to nonblocking */
        flags = fcntl(nn.fd, F_GETFL, 0);
        assert(flags != -1);
        rc = fcntl(nn.fd, F_SETFL, flags | O_NONBLOCK);
        assert(rc != -1);

        /* Setup work */
        wk.fn = net_read;
        wk.ctx = malloc(sizeof(net));
        assert(wk.ctx != NULL);
        net_move(&nn, wk.ctx); // TODO nn is no longer valid :(

        net* nnn = (net*) wk.ctx; // TODO Work around
        T1_CCAT(insert)(&nnn->lux, nnn->fd, wk /* moved */);
        printf("Accept\n");
        return;
    }
    assert(!(ev->events & EPOLLOUT));
    assert(!(ev->events & EPOLLERR));
    assert(!(ev->events & EPOLLHUP));
    assert(0);
}
void net_listen_inet(T1* lux, int port)
{
    net n;
    work wk;

    /* Setup net struct */
    T1_CCAT(move)(lux, &n.lux);
    n.addr.sin_family = AF_INET;
    n.addr.sin_port = htons(port);
    n.addr.sin_addr.s_addr = INADDR_ANY;// INADDR_LOOPBACK
    /* Socket */
    n.fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    /* Error checks */
    assert(n.fd > -1 || errno == EACCES);
    assert(n.fd > -1 || errno == EAFNOSUPPORT);
    assert(n.fd > -1 || errno == EINVAL);
    assert(n.fd > -1 || errno == EMFILE);
    assert(n.fd > -1 || errno == ENFILE);
    assert(n.fd > -1 || errno == ENOBUFS);
    assert(n.fd > -1 || errno == ENOMEM);
    assert(n.fd > -1 || errno == EPROTONOSUPPORT);
    assert(n.fd > -1);

    /* Bind */
    int rc;
    rc = bind(n.fd, (struct sockaddr*) &n.addr,
            sizeof(n.addr));

    assert(rc > -1 || errno == EACCES);
    assert(rc > -1 || errno == EADDRINUSE);
    assert(rc > -1 || errno == EBADF);
    assert(rc > -1 || errno == EINVAL);
    assert(rc > -1 || errno == ENOTSOCK);
    assert(rc > -1 || errno == EACCES);
    assert(rc > -1 || errno == EADDRNOTAVAIL);
    assert(rc > -1 || errno == EFAULT);
    assert(rc > -1 || errno == ELOOP);
    assert(rc > -1 || errno == ENAMETOOLONG);
    assert(rc > -1 || errno == ENOENT);
    assert(rc > -1 || errno == ENOMEM);
    assert(rc > -1 || errno == ENOTDIR);
    assert(rc > -1 || errno == EROFS);
    assert(rc > -1);

    /* Listen */
    rc = listen(n.fd, 100);
    assert(rc > -1 || errno == EADDRINUSE);
    assert(rc > -1 || errno == EBADF);
    assert(rc > -1 || errno == ENOTSOCK);
    assert(rc > -1 || errno == EOPNOTSUPP);
    assert(rc > -1);

    /* Setup work struct */
    wk.fn = net_accept;
    wk.ctx = malloc(sizeof(net));
    assert(wk.ctx != NULL);
    wk.ctx = &n; // TODO pass-by-const-ref

    /* Insert & exec */
    T1_CCAT(insert)(&n.lux, n.fd, wk /* moved */);
    net_exec(&n.lux); // TODO pass-by-const-ref
}
#undef T2_CCAT
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T2_PREFIX
#undef T1_PREFIX
#undef T2
#undef T1
/* End: Generic */

int start()
{
    linux_net lux;

    linux_net_construct(&lux /* construction */);
    net_listen_inet(&lux /* moved */, 8883);
}

int main()
{
    start();
}

