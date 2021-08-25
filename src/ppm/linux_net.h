#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)

typedef struct {
    int epoll;
    int original;
} linux_net;

void linux_net_construct(linux_net* lux)
{
    lux->epoll = epoll_create1(0);
    lux->original = 1;

    assert(lux->epoll > -1 || errno == EINVAL);
    assert(lux->epoll > -1 || errno == EMFILE);
    assert(lux->epoll > -1 || errno == ENFILE);
    assert(lux->epoll > -1 || errno == ENOMEM);
    assert(lux->epoll > -1);
}

void linux_net_deconstruct(linux_net* lux)
{
    int rc;

    if (lux->original == 0)
        return;

    if (lux->epoll > -1)
        rc = close(lux->epoll);

    assert(rc == 0 || errno == EBADF);
    assert(rc == 0 || errno == EINTR);
    assert(rc == 0 || errno == EIO);
    assert(rc == 0);
}

void linux_net_move(linux_net* cur, linux_net* other)
{
    other->epoll = cur->epoll;
    other->original = other->original;
    cur->epoll = -1;
}

void linux_net_copy(linux_net* cur, linux_net* other)
{
    other->epoll = cur->epoll;
    other->original = 0;
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

void linux_net_insert(linux_net* lux, int fd, T1_CCAT(ptr) wk /* move */)
{
    int rc;
    struct epoll_event ev;

    ev.events = EPOLLIN | EPOLLPRI;
                //EPOLLOUT
                //EPOLLET
    ev.data.ptr = wk.ptr;

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

void linux_net_replace(linux_net* lux, int fd, T1* wk, int write)
{
    int rc;
    struct epoll_event ev;

    ev.events = (write ? EPOLLOUT : EPOLLIN) |
                EPOLLPRI;
                //EPOLLOUT
                //EPOLLET
    ev.data.ptr = wk;

    rc = epoll_ctl(lux->epoll, EPOLL_CTL_MOD, fd, &ev);
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
