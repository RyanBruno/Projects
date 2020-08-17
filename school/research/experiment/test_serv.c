#include <rpc/rpc.h>
#include <stdio.h>

char* res = "World!";

char* helo(char* req)
{
    printf("%s\n", *(char**) req);
    return (char*) &res;
}

int main()
{
    registerrpc(1, 1, 1, helo, (xdrproc_t) xdr_string, (xdrproc_t) xdr_string);

    svc_run();
}
