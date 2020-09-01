#include <stdio.h>
#include "xdr_orset.c"

const char* hi = "Hello ";
const char* wrld = "world ";
const char* fb = "foobar ";

int test_xdr_orset_encode()
{
    struct orset os;
    struct orset* os_p;
    XDR xdr;
    char* mem;

    os_p = &os;
    orset_create(os_p, 1);
    orset_add(os_p, (void*) hi);
    orset_add(os_p, (void*) wrld);
    orset_add(os_p, (void*) fb);

    mem = (char*) malloc(20000);
    xdrmem_create(&xdr, mem, 20000, XDR_ENCODE);

    return xdr_orset(&xdr, &os_p) == 0;
}

void run_test(int(*fn)(), char* name)
{
    printf("[ ] Running \"%s\"...\n", name);

    if (!fn())
        printf("\033[F[P] Passed Test: %s\n", name);
    else
        printf("\033[F[F] Failed Test: %s\n", name);
}

int main()
{
    run_test(test_xdr_orset_encode, "[xdr_orset.c] Encode Test");
}
