#define PROCEDURE_NUMBER 100600
#define VERSION_NUMBER 1
#include <rpc/rpc.h>
#include <stdio.h>


struct merge_item {
    unsigned long k;
    char* v;
};

struct merge_message {
    unsigned int merge_len;
    struct merge_item mi[7000];
};

bool_t xdr_merge_item(XDR *xdr, void* item)
{
    struct merge_item* item_c = (struct merge_item*) item;

    if (xdr->x_op & XDR_DECODE)
        item_c->v = malloc(1024);

    if (xdr->x_op & XDR_FREE)
        free(item_c->v);

    return xdr_u_long(xdr, &(item_c->k)) && xdr_string(xdr, &(item_c->v), 1024);
}

bool_t xdr_merge_message(XDR *xdr, void* msg)
{
    struct merge_message* msg_c = (struct merge_message*) msg;

    return xdr_array(xdr, (char**) &(msg_c->mi), &(msg_c->merge_len), 7000, sizeof(struct merge_item), (xdrproc_t) xdr_merge_item);
}

void rpc_merge_request(struct svc_req *req, SVCXPRT *xprt)
{

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
