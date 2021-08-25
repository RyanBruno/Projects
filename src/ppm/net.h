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

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
#define T2_CCAT(x) CCAT2(T2_PREFIX, x)
#define T3_CCAT(x) CCAT2(T3_PREFIX, x)
#define T4_CCAT(x) CCAT2(T4_PREFIX, x)
#define T5_CCAT(x) CCAT2(T5_PREFIX, x)
struct net {
    T1 lux;
    int fd;
    struct sockaddr_in addr;
    T3 wk;
    T4 wbuf;
    T5 ctx;
    int deconstructed;
};

#define T0 struct net
#define T0_PREFIX net_
#define T01 struct work
#define T01_PREFIX work_
#include "ptr.h"

void net_construct(struct net* n)
{
    T1_CCAT(construct)(&n->lux);
    n->fd = -1;
    n->wk.ctx = n;
    T4_CCAT(construct)(&n->wbuf, NULL, 0);
    T5_CCAT(construct)(&n->ctx);
    n->deconstructed = 0;
}

void net_deconstruct(struct net* n)
{
    int rc;

    if (n->deconstructed) return;
    n->deconstructed = 1;

    T1_CCAT(deconstruct)(&n->lux);
    if (n->fd > -1) rc = close(n->fd);
    assert(rc == 0 || errno == EBADF);
    assert(rc == 0 || errno == EINTR);
    assert(rc == 0 || errno == EIO);
    assert(rc == 0);
    T3_CCAT(deconstruct)(&n->wk);
    T4_CCAT(deconstruct)(&n->wbuf);
    T5_CCAT(deconstruct)(&n->ctx);
}

void net_copy(struct net* cur, struct net* other)
{
    T1_CCAT(copy)(&cur->lux, &other->lux);
    other->fd = -1;
    other->wk.ctx = other;
    T4_CCAT(construct)(&other->wbuf, NULL, 0);
    T5_CCAT(construct)(&other->ctx);
    other->deconstructed = 0;
}

struct net net_move(struct net* n /* move */)
{
    struct net other;

    memcpy(&other, n, sizeof(struct net));
    n->wk.ctx = n;
    memset(n, '\0', sizeof(struct net));

    return other;
}

void net_move_mem(struct net* cur /* move */, struct net* other)
{
    memcpy(other, cur, sizeof(struct net));
    other->wk.ctx = other;
    memset(cur, '\0', sizeof(struct net));
}

void net_exec(struct net* n /* move */)
{
    int num;
    T2 e[10];

    for (;;) {
        num = T1_CCAT(select)(&n->lux, (T2*) &e, 10, 100000);

        for (int i = 0; i < num; i++) {
            T2_CCAT(exec)(
                    &e[i] /* const-ref */,
                    NULL);
        }
    }

    //net_deconstruct(n); // TODO fix
}

void net_write(void* ctx, const void* v /* const-ref */)
{
    struct net* n = (struct net*) ctx;
    const T2* ev = (T2*) v;

    write(n->fd, T4_CCAT(str)(&n->wbuf),
            T4_CCAT(size)(&n->wbuf));
    /* next steps */
    exit(0);
}

void net_write_all(struct net* n, T4* b /* const-ref */)
{
    /* Copy bytes */
    T4_CCAT(insert)(&n->wbuf,
            T4_CCAT(str)(b),
            T4_CCAT(size)(b));

    /* Setup work struct */
    n->wk.fn = net_write;

    /* Insert & exec */
    T1_CCAT(replace)(&n->lux, n->fd, ptr_move(&n->wk), 1);
}

void net_read(void* ctx, const void *v /* const-ref */)
{
    struct net* n = (struct net*) ctx;
    const T2* ev = (T2*) v;

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

        // TODO optional modules
        T5_CCAT(insert)(&n->ctx, b, s);

        if (T5_CCAT(ready)(&n->ctx)) {
            net_write_all(n, T5_CCAT(get)(&n->ctx));
        }
        return;
    }
    assert(!(ev->events & EPOLLOUT));
    assert(!(ev->events & EPOLLERR));
    assert(!(ev->events & EPOLLHUP));
    assert(0);
}

void net_accept(void* ctx, const void* v /* const-ref */)
{
    struct net* n = (struct net*) ctx;
    const T2* ev = (T2*) v;

    if (ev->events & EPOLLIN || ev->events & EPOLLPRI) {
        net_ptr nn;
        socklen_t addr_len = sizeof(n->addr);
        int rc;
        int flags;

        /* Malloc new net struct */
        net_ptr_construct(&nn, malloc(sizeof(struct net)));
        assert(nn.ptr != NULL);

        /* Setup new net struct */
        net_copy(n, nn.ptr);

        /* Accept */
        nn.ptr->fd = accept(n->fd, (struct sockaddr*) &nn.ptr->addr, &addr_len);

        if (nn.ptr->fd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        assert(nn.ptr->fd > -1 || errno == EBADF);
        assert(nn.ptr->fd > -1 || errno == ECONNABORTED);
        assert(nn.ptr->fd > -1 || errno == EINTR);
        assert(nn.ptr->fd > -1 || errno == EINVAL);
        assert(nn.ptr->fd > -1 || errno == EMFILE);
        assert(nn.ptr->fd > -1 || errno == ENFILE);
        assert(nn.ptr->fd > -1 || errno == ENOBUFS);
        assert(nn.ptr->fd > -1 || errno == ENOMEM);
        assert(nn.ptr->fd > -1 || errno == ENOTSOCK);
        assert(nn.ptr->fd > -1 || errno == EOPNOTSUPP);
        assert(nn.ptr->fd > -1 || errno == EPROTO);
        assert(nn.ptr->fd > -1);

        /* Set socket to nonblocking */
        flags = fcntl(nn.ptr->fd, F_GETFL, 0);
        assert(flags != -1);
        rc = fcntl(nn.ptr->fd, F_SETFL, flags | O_NONBLOCK);
        assert(rc != -1);

        /* Setup work */
        nn.ptr->wk.fn = net_read;

        T1_CCAT(insert)(&nn.ptr->lux,
                        nn.ptr->fd,
                        net_ptr_morph(&nn, &nn.ptr->wk));
        printf("Accept\n");
        return;
    }
    assert(!(ev->events & EPOLLOUT));
    assert(!(ev->events & EPOLLERR));
    assert(!(ev->events & EPOLLHUP));
    assert(0);
}

void net_listen_inet(int port)
{
    struct net n;
    T3_CCAT(ptr) wk;

    /* Setup net struct */
    net_construct(&n);
    T3_CCAT(ptr_construct)(&wk, &n.wk);

    /* Setup SockAddr */
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
    n.wk.fn = net_accept;

    /* Insert & exec */
    T1_CCAT(insert)(&n.lux, n.fd, T3_CCAT(ptr_move)(&wk));
    net_exec(&n /* TODO moved */); /* TODO fix (n is invalid) */
}
#undef T5_CCAT
#undef T4_CCAT
#undef T3_CCAT
#undef T2_CCAT
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T2_PREFIX
#undef T1_PREFIX
#undef T5
#undef T4
#undef T3
#undef T2
#undef T1
