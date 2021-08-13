#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Buf */
typedef struct {
    size_t s;
    size_t c;
    void* b;
} buf;

void buf_construct(buf* b, void* v, size_t s)
{
    b->s = s;
    b->c = s;
    b->b = malloc(s);
    assert(b->b != NULL);

    memcpy(b->b, v, s);
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
/* End: Buf */
/* HTTP */
#define T1 buf
#define T1_PREFIX buf_

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
typedef struct {
    T1 buf;
    char* method;
    char* uri;
    char* version;
} http;

void http_construct(http* h)
{
    T1_CCAT(construct)(&h->buf, NULL, 0);
    h->method = NULL;
    h->uri = NULL;
    h->version = NULL;
}
void http_insert(http* h, T1 b)
{
    char* s;
    char* p;
    size_t i;

    T1_CCAT(insert)(&h->buf, T1_CCAT(str)(&b), T1_CCAT(size)(&b));
    s = T1_CCAT(str)(&h->buf);
    p = s;

    h->method = s;

    for (i = 0; i < T1_CCAT(size)(&h->buf) && s[i] != ' ' && s[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->method == NULL) h->method = strndup(p, i);
    p += ++i;

    for (; i < T1_CCAT(size)(&h->buf) && s[i] != ' ' && s[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->uri == NULL) h->uri = strndup(p, i - (p - s));
    p += ++i - (p - s);

    if (s[i - 1] == ' ') {
        for (; i < T1_CCAT(size)(&h->buf) && s[i] != ' ' && s[i] != '\r'; i++) {}
        if (i >= T1_CCAT(size)(&h->buf)) return;

        if (h->version == NULL) h->version = strndup(p, i - (p - s));
        p += ++i - (p - s);
    }
}
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
/* End: HTTP */

int main()
{
    http h;
    buf b1;
    buf b2;
    buf b3;

    http_construct(&h);

    buf_construct(&b1, "GET", 3);
    buf_construct(&b2, " /hello", 7);
    buf_construct(&b3, "-world HTTP/1.1\r", 17);

    http_insert(&h, buf_move(&b1));
    printf("Break\n");
    http_insert(&h, buf_move(&b2));
    printf("Break\n");
    http_insert(&h, buf_move(&b3));
    printf("Method: -%s-\n", h.method);
    printf("URI: -%s-\n", h.uri);
    printf("Version: -%s-\n", h.version);
    printf("End\n");

    return 0;
}
