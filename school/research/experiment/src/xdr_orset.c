#include "orset.h"
#include <rpc/rpc.h>
#include <stdio.h>

bool_t xdr_orset_item(XDR *xdr, void* os)
{
    struct orset** os_c = (struct orset**) os;

    if (xdr->x_op == XDR_DECODE) {
        unsigned long k;
        char* s;

        if ((s = (char*) malloc(1024)) == NULL)
            printf("Malloc():\n");

        if (!xdr_u_long(xdr, &k))
            printf("xdr_u_long():\n");

        if (!xdr_string(xdr, &s, 1024))
            printf("xdr_string():\n");

        if (s[0] == '\0') {
            orset_remove(*os_c, k);
            return 1;
        }

        orset_add(*os_c, (char*) s);
    }

    if (xdr->x_op == XDR_ENCODE) {
        struct unordered_map_pair ump;

        unordered_map_next(*os_c, &ump);
        /*printf("%s\n", ump.i);*/

        if (!xdr_u_long(xdr, &ump.k))
            printf("xdr_u_long():\n");

        if (!xdr_string(xdr, (char**) &ump.i, 1024))
            printf("xdr_string():\n");
        return 1;
    }

    /*if (xdr->x_op == XDR_FREE) {
    }*/
    return 0;
}

bool_t xdr_orset(XDR *xdr, void* os)
{
    struct orset** os_c = (struct orset**) os;
    unsigned int l;

    if (xdr->x_op == XDR_DECODE) {
        *os_c = malloc(sizeof(struct orset));
        orset_create(*os_c, 1);
    }

    if (xdr->x_op == XDR_ENCODE) {
        unordered_map_reset((*os_c)->os_map);
        l = 3; // TODO
    }

    /*if (xdr->x_op == XDR_FREE) {
    }*/

    return xdr_u_short(xdr,
            &(*os_c)->os_node_id)
        && xdr_array(xdr,
            (char**) os_c,
            &l,
            999999999,
            0,
            (xdrproc_t) xdr_orset_item);
}
