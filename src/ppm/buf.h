#include <stddef.h>

typedef struct {
    size_t s;
    size_t c;
    void* b;
} buf;

void buf_construct(buf* b, void* v, size_t s);
buf buf_move(buf* cur);
void buf_insert(buf* b, void* v, size_t s);
void buf_clear(buf* b);
size_t buf_size(buf* b);
char* buf_str(buf* b);
