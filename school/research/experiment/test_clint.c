#include <rpc/rpc.h>
#include <stdio.h>

char str[64];

int main(int argc, char* argv[])
{
    char* helo = "Hello";
    char wrld[64];
    char* wrld_p = (char*) &wrld;

    callrpc(argv[1], 1, 1, 1, (xdrproc_t) xdr_string, &helo, (xdrproc_t) xdr_string, &wrld_p);


    printf("%s\n", wrld);
}
