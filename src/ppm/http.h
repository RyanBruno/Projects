#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#if 0
#define T1 buf
#define T1_PREFIX buf_
#endif

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)

typedef struct {
    T1 buf;
    char* method;
    char* uri;
    char* version;
    T1 res_buf;
    int res_buf_ready;
} http;

void http_construct(http* h)
{
    T1_CCAT(construct)(&h->buf, NULL, 0);
    h->method = NULL;
    h->uri = NULL;
    h->version = NULL;
    h->res_buf_ready = 0;
}

int http_ready(http* h)
{
    return h->res_buf_ready;
}

T1* /* const-ref */
http_get(http* h)
{
    return &h->res_buf;
}

void http_insert(http* h, T1* b /* const-ref */)
{
    char* s;
    char* p;
    size_t i;

    T1_CCAT(insert)(&h->buf, T1_CCAT(str)(b), T1_CCAT(size)(b));
    s = T1_CCAT(str)(&h->buf);
    p = s;

    h->method = s;

    for (i = 0; i < T1_CCAT(size)(&h->buf) && s[i] != ' ' && s[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->method == NULL) h->method = strndup(p, i);
    p += ++i;

    for (; i < T1_CCAT(size)(&h->buf) && s[i] != ' ' && s[i] != '\r'; i++) {}
    if (i >= T1_CCAT(size)(&h->buf)) return;

    if (h->uri == NULL) {
        T1_CCAT(construct)(&h->res_buf, "HTTP/1.1 200 OK\r\n\r\n", 19);
        h->res_buf_ready = 1;

        h->uri = strndup(p, i - (p - s));
        printf("URI %s\n", h->uri);
    }
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

#if 0
int test_main2()
{
    http h;
    buf b1;
    buf b2;
    buf b3;

    http_construct(&h);

    buf_construct(&b1, "GET", 3);
    buf_construct(&b2, " /hello", 7);
    buf_construct(&b3, "-world HTTP/1.1\r", 17);

    /*http_insert(&h, buf_move(&b1));
    printf("Break\n");
    http_insert(&h, buf_move(&b2));
    printf("Break\n");
    http_insert(&h, buf_move(&b3));*/
    printf("Method: -%s-\n", h.method);
    printf("URI: -%s-\n", h.uri);
    printf("Version: -%s-\n", h.version);
    printf("End\n");

    return 0;
}
#endif
