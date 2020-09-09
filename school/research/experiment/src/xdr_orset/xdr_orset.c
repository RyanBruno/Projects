/* xdr routines transform C data into data
 * that can be sent as RPC parameters or
 * returns. xdr_orset is for OrSets. See
 * orset.h.
 */
#include "xdr_orset.h"
#include "../node.h"
#include "../orset/orset.h"

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
        char* i;

        if ((i = malloc(1024)) == NULL)
            printf("Malloc():\n");

        if (!xdr_u_longlong_t(xdr, &k))
            printf("xdr_u_long():\n");

        if (!xdr_string(xdr, &i, 1024))
            printf("xdr_string():\n");

        if (i[0] == '\0') {
            free(i);
            orset_remove(os_c, k);
            return 1;
        }

        unordered_map_add(os_c->os_map, k, (void*) i);
    }

    if (xdr->x_op == XDR_ENCODE) {
        unsigned long k;
        void* i;

        unordered_map_next(os_c->os_map, &k, &i);

        /* Write the key */
        if (!xdr_u_longlong_t(xdr, &k))
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

    /*if (xdr->x_op == XDR_FREE) { */
        return 1;
    /*}*/
    return 0;
}

/* OrSets encode to and decode from  an
 * unsigned long representing the node is
 * then an array of xdr_orset_items (see
 * above).
 */
bool_t xdr_orset(XDR *xdr, void* os)
{
    bool_t rc;
    struct orset* os_c = (struct orset*) os;
    size_t l;

    if (xdr->x_op == XDR_DECODE)
        orset_create(os_c, 1);

    if (xdr->x_op == XDR_ENCODE) {
        sem_wait(&os_sem);
        unordered_map_reset(os_c->os_map);
        l = unordered_map_size(os_c->os_map);
    }

    if (xdr->x_op == XDR_FREE) {
        // TODO unordered_map_free
    }

    rc = xdr_u_short(xdr,
            &os_c->os_node_id)
        && xdr_array(xdr,
            (char**) &os_c,
            (u_int*) &l,
            999999999,
            0,
            (xdrproc_t) xdr_orset_item);

    if (xdr->x_op == XDR_ENCODE)
        sem_post(&os_sem);

    return rc;
}
