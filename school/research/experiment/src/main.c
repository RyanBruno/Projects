#define PROCEDURE_NUMBER 100600
#define VERSION_NUMBER 1
#define MAX_MERGE_ITEMS 102400
#include <rpc/rpc.h>
#include <stdio.h>


struct merge_item {
    unsigned int k;
    char* v;
};

struct merge_message {
    unsigned int merge_len;
    struct merge_item mi[MAX_MERGE_ITEMS];
};

bool_t xdr_merge_item(XDR *xdr, void* item)
{
    struct merge_item* item_c = (struct merge_item*) item;

    if (xdr->x_op & XDR_DECODE) {
        if ((item_c->v = (char*) malloc(1024)) == NULL) {
            printf("Malloc\n");
            return 0;
        }
    }
        

    if (xdr->x_op & XDR_FREE) {
        printf("FREE: %s\n", item_c->v);
        free(item_c->v);
        printf("POST FREE:\n");
        return 1;
    }

    return xdr_u_int(xdr,
            (unsigned int*) &(item_c)->k)
        && xdr_string(xdr,
            (char**) &(item_c)->v,
            1024);
}

bool_t xdr_merge_message(XDR *xdr, void* msg)
{
    struct merge_message** msg_c = (struct merge_message**) msg;
    struct merge_item* mi_p = (struct merge_item*) &(*msg_c)->mi;

    if (xdr->x_op & XDR_FREE) {
        printf("LENGTH 2: %d\n", (*msg_c)->merge_len);
        printf("FREE: %s\n", (*msg_c)->mi[0].v);
    }

    return xdr_array(xdr,
            (char**) &mi_p,
            (unsigned int*) &(*msg_c)->merge_len,
            MAX_MERGE_ITEMS,
            sizeof(struct merge_item),
            (xdrproc_t) xdr_merge_item);
}

void rpc_merge_request(struct svc_req *req, SVCXPRT *xprt)
{
    struct merge_message mi;
    struct merge_message* mi_p = &mi;
    int rc = 6;

    if (!svc_getargs(xprt, (xdrproc_t) xdr_merge_message, (char*) &mi_p)) {
        printf("Mission Failed!\n");
        svc_sendreply(xprt, (xdrproc_t) xdr_int, (char*) &rc);
        return;
    }
    //svc_freeargs(xprt, (xdrproc_t) xdr_merge_message, (char*) &mi_p);
    printf("WE MADE IT\n");
    for (int i = 0; i < mi.merge_len; i++) {
        printf("%d %s\n", mi.mi[i].k, mi.mi[i].v);
    }

    svc_sendreply(xprt, (xdrproc_t) xdr_int, (char*) &rc);
}

int register_procedure()
{
    int rc; 
    SVCXPRT* xprt;

    if ((xprt = svctcp_create(RPC_ANYSOCK, 0, 0)) == NULL)
        return -1;

    pmap_unset(PROCEDURE_NUMBER, VERSION_NUMBER);
    if (!svc_register(xprt, PROCEDURE_NUMBER, VERSION_NUMBER, rpc_merge_request, IPPROTO_TCP))
        return -1;

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        printf("Client\n");
        int rc;
        struct merge_message ms = {
            .merge_len=3,
            .mi={
                { .k=1, .v="Hello" },
                { .k=2, .v="World" },
                { .k=0, .v="George" }
            }
        };

        struct merge_message* ms_p = &ms;
        struct timeval to;
        to.tv_sec = 20;

        CLIENT* client = clnt_create("127.0.0.1", 100600, 1, "tcp");
        clnt_call(client, 1, (xdrproc_t) xdr_merge_message, &ms_p, (xdrproc_t) xdr_int, &rc, to);
        printf("%d\n", rc);
        return 0;

    }

    printf("Server\n");

    if (register_procedure() == -1) {
        printf("Could not register\n");
        exit(-1);
    }

    svc_run();
    return 0;
}
