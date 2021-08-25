#include <string.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)

struct work {
    void (*fn)(void*, const void*);
    T1* ctx;
};

#define T0 struct work
#define T0_PREFIX work_
#include "ptr.h"

void work_deconstruct(struct work* wk)
{
    //T1_CCAT(deconstruct)(wk->ctx);
}

void work_move_mem(struct work* cur, struct work* other)
{
    memcpy(other, cur, sizeof(struct work));
}

struct work work_move(struct work* cur)
{
    struct work other;

    memcpy(&other, cur, sizeof(struct work));

    return other;
}

void work_exec(struct work* w, void* v)
{
    w->fn(w->ctx, v);
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
