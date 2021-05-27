
#ifdef TESTS_INTERNAL
int test_xdr_orset_encode()
{
    struct orset os;
    XDR xdr;
    char* mem;

    orset_create(&os, 1);
    orset_add(&os, (void*) hi);
    orset_add(&os, (void*) wrld);
    orset_add(&os, (void*) fb);

    mem = (char*) malloc(20000);
    xdrmem_create(&xdr, mem, 20000, XDR_ENCODE);

    return xdr_orset(&xdr, &os) == 0;
}
#endif
