#include "buf.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Buf */
void buf_construct(buf* b, void* v, size_t s)
{
    b->s = s;
    b->c = s;
    b->b = malloc(s);
    assert(b->b != NULL);

    memcpy(b->b, v, s);
}

void buf_deconstruct(buf* b)
{
    free(b->b);
    b->s = 0;
    b->c = 0;
    b->b = NULL;
}

buf buf_move(buf* cur)
{
    buf other;

    other.s = cur->s;
    other.c = cur->c;
    other.b = cur->b;

    cur->s = 0;
    cur->c = 0;
    cur->b = NULL;

    return other;
}
void buf_insert(buf* b, void* v, size_t s)
{
    if ((b->s + s) > b->c) {
        void* t;
        t = realloc(b->b, b->s + s);
        assert(t != NULL);
        b->b = t;
        b->c += s;
    }

    memcpy(b->b + b->s, v, s);
    b->s += s;
}
void buf_clear(buf* b)
{
    b->s = 0;
}
size_t buf_size(buf* b)
{
    return b->s;
}
char* buf_str(buf* b)
{
    return b->b;
}
