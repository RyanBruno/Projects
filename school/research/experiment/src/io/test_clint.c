#include <rpc/rpc.h>
#include <stdio.h>

char str[64];

int main(int argc, char* argv[])
{
    char* helo = "Hello";
    char wrld[64];
    char* wrld_p = (char*) &wrld;
    int rc;
    struct timeval to;
    to.tv_sec = 20;

    CLIENT* client = clnt_create(argv[1], 100600, 1, "tcp");
    clnt_call(client, 1, (xdrproc_t) xdr_string, &helo, (xdrproc_t) xdr_string, &wrld_p, to);


    printf("%s\n", wrld);
}
