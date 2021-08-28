#include <stddef.h>

typedef struct {
    size_t s;
    size_t c;
    void* b;
    void* rptr;
} buf;

void buf_construct(buf* b, const void* v, size_t s);
void buf_deconstruct(buf* b);
buf buf_move(buf* cur);
void buf_insert(buf* b, const void* v, size_t s);
void buf_clear(buf* b);
void buf_read(buf* b, size_t s);
size_t buf_size(buf* b);
char* buf_str(buf* b);
