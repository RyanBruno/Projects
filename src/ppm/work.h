#include <string.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)

typedef struct {
    void (*fn)(void*, const void*);
    struct T1* ctx;
} work;

void work_move_mem(work* cur, work* other)
{
    memcpy(other, cur, sizeof(work));
}

work work_move(work* cur)
{
    work other;

    memcpy(&other, cur, sizeof(work));

    return other;
}

void work_exec(work* w, void* v)
{
    w->fn(w->ctx, v);
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
