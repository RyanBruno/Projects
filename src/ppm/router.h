#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(T1_PREFIX, x)

struct router {
    short int res_code;
    T1 res_buf;
};

void
router_construct(struct router* r)
{
    r->res_code = 0;
    T1_CCAT(construct)(&r->res_buf, NULL, 0);
}

void
router_deconstruct(struct router* r)
{
    T1_CCAT(deconstruct)(&r->res_buf);
}

int
router_ready(struct router* r)
{
    return r->res_code != 0;
}

T1*
router_get(struct router* r)
{
    return &r->res_buf;
}

void
router_insert(struct router* r, const char* uri)
{
    void *dlhandle;
    const char* (*symbol)(const char*);
    int rc;
    const char* res;
    char* dl;

    dl = malloc(4096);
    assert(dl != NULL);
    snprintf(dl, 4096, "./%s", uri + 1);

    printf("Opening: %s\n", dl);
    dlhandle = dlopen(dl, RTLD_NOW | RTLD_LOCAL);

    if (dlhandle == NULL) printf("%s\n", dlerror());
    assert(dlhandle != NULL);

    symbol = dlsym(dlhandle, "hello_world");

    if (symbol == NULL) printf("%s\n", dlerror());
    assert(symbol != NULL);


    res = symbol(uri);
    r->res_code = 200;
    T1_CCAT(insert)(&r->res_buf, res, strlen(res));
    free((void*)res);

    rc = dlclose(dlhandle);
    assert(rc == 0);
}

#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T1_PREFIX
#undef T1
