#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)
#define T2_CCAT(x) CCAT2(T2_PREFIX, x)

typedef struct {
    T1 buf;
    char* method;
    char* uri;
    char* version;
    //T1 res_buf;
    //int res_buf_ready;
    T2 ctx;
} http;

void http_construct(http* h)
{
    T1_CCAT(construct)(&h->buf, NULL, 0);
    h->method = NULL;
    h->uri = NULL;
    h->version = NULL;
    //h->res_buf_ready = 0;
    T2_CCAT(construct)(&h->ctx);
}

void http_deconstruct(http* h)
{
    T1_CCAT(deconstruct)(&h->buf);
    if (h->method != NULL) free(h->method);
    if (h->uri != NULL) free(h->uri);
    if (h->version != NULL) free(h->version);
    T2_CCAT(deconstruct)(&h->ctx);
}

int http_ready(http* h)
{
    return T2_CCAT(ready)(&h->ctx);
}

T1* /* const-ref */
http_get(http* h)
{
    return T2_CCAT(get)(&h->ctx);
}

void http_insert(http* h, char* b /* const-ref */, size_t s)
{
    char* str;
    char* p;
    size_t i;

    T1_CCAT(insert)(&h->buf, b, s);
    str = T1_CCAT(str)(&h->buf);
    p = str;

    for (i = 0; i < T1_CCAT(size)(&h->buf) && str[i] != ' ' && str[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->method == NULL) h->method = strndup(p, i);
    p += ++i;

    for (; i < T1_CCAT(size)(&h->buf) && str[i] != ' ' && str[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->uri == NULL) {
        h->uri = strndup(p, i - (p - str));
        T2_CCAT(insert)(&h->ctx, h->uri);
        return;
    }
    p += ++i - (p - str);

    if (str[i - 1] == ' ') {
        for (; i < T1_CCAT(size)(&h->buf) && str[i] != ' ' && str[i] != '\r'; i++) {}
        if (i >= T1_CCAT(size)(&h->buf)) return;

        if (h->version == NULL) h->version = strndup(p, i - (p - str));
        p += ++i - (p - str);
    }
}
#undef T2_CCAT
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T2_PREFIX
#undef T1_PREFIX
#undef T2
#undef T1
