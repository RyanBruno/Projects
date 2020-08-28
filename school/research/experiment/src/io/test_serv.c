#include <rpc/rpc.h>
#include <stdio.h>

char* res = "World!";

void helo(struct svc_req *request, SVCXPRT *xprt)
{
    char msg[64];

    svc_getargs(xprt, (xdrproc_t) xdr_string, &msg);

    printf("%s\n", *(char**) msg);
    svc_sendreply(xprt, (xdrproc_t) xdr_string, (char*) &res);
}

int main()
{
    int rc; 
    SVCXPRT* xprt;

    xprt = svctcp_create(RPC_ANYSOCK, 1024, 1024);
    pmap_unset(100600, 1);
    rc = svc_register(xprt, 100600, 1, helo, IPPROTO_TCP);

    printf("%d\n", rc);

    svc_run();
}
