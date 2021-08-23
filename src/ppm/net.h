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
};
typedef struct net net;

void net_construct(net* n)
{
    T1_CCAT(construct)(&n->lux);
    n->fd = -1;
    n->wk.ctx = n;
    T4_CCAT(construct)(&n->wbuf, NULL, 0);
    T5_CCAT(construct)(&n->ctx);
}

void net_copy(net* cur, net* other)
{
    T1_CCAT(copy)(&cur->lux, &other->lux);
    other->fd = -1;
    other->wk.ctx = other;
    T4_CCAT(construct)(&other->wbuf, NULL, 0);
    T5_CCAT(construct)(&other->ctx);
}

net net_move(net* n /* move */)
{
    net other;

    memcpy(&other, n, sizeof(net));
    n->wk.ctx = n;
    memset(n, '\0', sizeof(net));

    return other;
}

void net_move_mem(net* cur /* move */, net* other)
{
    memcpy(other, cur, sizeof(net));
    other->wk.ctx = other;
    memset(cur, '\0', sizeof(net));
}

void net_deconstruct(net* n)
{
    // TODO
}

void net_exec(net* n /* move */)
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
    net* n = (net*) ctx;
    const T2* ev = (T2*) v;

    write(n->fd, T4_CCAT(str)(&n->wbuf),
            T4_CCAT(size)(&n->wbuf));
    exit(0);
}

void net_write_all(net* n, T4 b /* moved */)
{
    /* Copy bytes */
    T4_CCAT(insert)(&n->wbuf,
            T4_CCAT(str)(&b),
            T4_CCAT(size)(&b));

    /* Setup work struct */
    n->wk.fn = net_write;

    /* Insert & exec */
    T1_CCAT(replace)(&n->lux, n->fd, ptr_move(&n->wk), 1);

    // TODO deconstruct b
}

void net_read(void* ctx, const void *v /* const-ref */)
{
    net* n = (net*) ctx;
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

        // TODO detect modules and push
        T4 buf;
        buf_construct(&buf, b, s);
        T5_CCAT(insert)(&n->ctx, &buf);

        if (T5_CCAT(ready)(&n->ctx)) {
            // pass in wk?
            net_write_all(n, T4_CCAT(move)(T5_CCAT(get)(&n->ctx)));
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
    net* n = (net*) ctx;
    const T2* ev = (T2*) v;

    if (ev->events & EPOLLIN || ev->events & EPOLLPRI) {
        net* nn;
        socklen_t addr_len = sizeof(n->addr);
        int rc;
        int flags;

        /* Malloc new net struct */
        nn = malloc(sizeof(net));
        assert(nn != NULL);

        /* Setup new net struct */
        net_copy(n, nn);

        /* Accept */
        nn->fd = accept(n->fd, (struct sockaddr*) &nn->addr, &addr_len);

        if (nn->fd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        assert(nn->fd > -1 || errno == EBADF);
        assert(nn->fd > -1 || errno == ECONNABORTED);
        assert(nn->fd > -1 || errno == EINTR);
        assert(nn->fd > -1 || errno == EINVAL);
        assert(nn->fd > -1 || errno == EMFILE);
        assert(nn->fd > -1 || errno == ENFILE);
        assert(nn->fd > -1 || errno == ENOBUFS);
        assert(nn->fd > -1 || errno == ENOMEM);
        assert(nn->fd > -1 || errno == ENOTSOCK);
        assert(nn->fd > -1 || errno == EOPNOTSUPP);
        assert(nn->fd > -1 || errno == EPROTO);
        assert(nn->fd > -1);

        /* Set socket to nonblocking */
        flags = fcntl(nn->fd, F_GETFL, 0);
        assert(flags != -1);
        rc = fcntl(nn->fd, F_SETFL, flags | O_NONBLOCK);
        assert(rc != -1);

        /* Setup work */
        nn->wk.fn = net_read;

        T1_CCAT(insert)(&nn->lux, nn->fd, ptr_move(&nn->w));
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
    net n;

    /* Setup net struct */
    net_construct(&n);

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
    T1_CCAT(insert)(&n.lux, n.fd, ptr_move(&n.wk));
    net_exec(&n /* TODO moved */); /* TODO fix (n is invalid */
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
