#include <sys/epoll.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
void epoll_event_exec(struct epoll_event* ev, void* v)
{
    (void) v;
    T1_CCAT(exec)(ev->data.ptr, ev);
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
