#include "../orset/orset.h"
#include <rpc/xdr.h>
#include <stdio.h>

static char* NULL_STRING = "\0";

/* OrSet items encode and decode to an
 * unsigned long key then a string as data.
 * Strings are held using buffers created by
 * malloc on decode. Tombstones are encoded
 * by a zero length string.
 */
bool_t xdr_orset_item(XDR *xdr, void* os)
{
    struct orset* os_c = (struct orset*) os;

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
            free(s);
            orset_remove(os_c, k);
            return 1;
        }

        unordered_map_add(os_c, k, s);
    }

    if (xdr->x_op == XDR_ENCODE) {
        unsigned long k;
        void* i;

        unordered_map_next(os_c, &k, &i);

        /* Write the key */
        if (!xdr_u_long(xdr, &k))
            printf("xdr_u_long():\n");

        /* Write '\0' if item is a tombstone. */
        if (orset_is_tombstone(os_c, i)) {
            if (!xdr_string(xdr, &NULL_STRING, 1024))
                printf("xdr_string():\n");
            return 1;
        }

        /* Write the string item */
        if (!xdr_string(xdr, (char**) &i, 1024))
            printf("xdr_string():\n");
        return 1;
    }

    /*if (xdr->x_op == XDR_FREE) {
    }*/
    return 0;
}

/* OrSets encode to and decode from ~an
 *~unsigned~long~representing~the~node~is~
 * then an array of xdr_orset_items (see
 * above).
 */
bool_t xdr_orset(XDR *xdr, void* os)
{
    struct orset* os_c = (struct orset*) os;
    unsigned int l;

    if (xdr->x_op == XDR_DECODE)
        orset_create(os_c, 1);

    if (xdr->x_op == XDR_ENCODE) {
        unordered_map_reset(os_c->os_map);
        l = 3; // TODO
    }

    /*if (xdr->x_op == XDR_FREE) {
    }*/

    /*return xdr_u_short(xdr,
            &(*os_c)->os_node_id)
        && */
    return xdr_array(xdr,
            (char**) os_c,
            &l,
            999999999,
            0,
            (xdrproc_t) xdr_orset_item);
}
