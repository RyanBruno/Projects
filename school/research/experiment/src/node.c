#define PROGRAM_NUMBER 100600
#define VERSION_NUMBER 1
#define MAX_MERGE_ITEMS 102400
#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "xdr_orset.c"

#define NODE_ID 1

struct orset os;
sem_t os_sem;

void rpc_merge_request(struct svc_req *req, SVCXPRT *xprt)
{
    struct orset* rmt_os;

    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: Invalid arguments.\n");
        return;
    }

    orset_merge(&os, rmt_os);

    if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: free\n");
        exit(-1);
        return;
    }
}

int register_procedure()
{
    int rc;
    SVCXPRT* xprt;

    if ((xprt = svctcp_create(RPC_ANYSOCK, 0, 0)) == NULL)
        return -1;

    pmap_unset(PROGRAM_NUMBER, VERSION_NUMBER);
    if (!svc_register(xprt, PROGRAM_NUMBER, VERSION_NUMBER, rpc_merge_request, IPPROTO_TCP))
        return -1;

    return 0;
}

void* client_thread_fn(void* os)
{
    struct orset* os_c = (struct orset*) os;

    while (1) {
        sleep(60);
        int rc;
        struct timeval to;

        to.tv_sec = 20;

        CLIENT* client = clnt_create("127.0.0.1", PROGRAM_NUMBER, VERSION_NUMBER, "tcp");
        clnt_call(client, 1, (xdrproc_t) xdr_orset, &os_c, (xdrproc_t) xdr_int, &rc, to);
        printf("%d\n", rc);

        sleep(60);
    }
}

int main(int argc, char* argv[])
{
    pthread_t client_thread;
    pthread_attr_t client_thread_attr;

    /* Create our orset */
    orset_create(&os, NODE_ID);

    if (sem_init(&os_sem, 0, 1)) {
        printf("sem_init():\n");
        exit(-1);
    }

    if (pthread_create(&client_thread,
                NULL,
                client_thread_fn,
                &os))
    {
        printf("pthread_create():\n");
        exit(-1);
    }


    /* Register our service */
    if (register_procedure()) {
        printf("register_procedure():\n");
        exit(-1);
    }

    svc_run();

    return -1;
}
