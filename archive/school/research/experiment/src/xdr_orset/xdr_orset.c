/* xdr routines transform C data into data
 * that can be sent as RPC parameters or
 * returns. xdr_orset is for OrSets. See
 * orset.h.
 */
#include "xdr_orset.h"
#include "../node.h"
#include "../orset/orset.h"

static char* NULL_STRING = "\0";

struct xdr_orset_item_cur {
    struct orset* os;
    uint64_t k;
    void* i;
};

/* OrSet items encode and decode to an
 * unsigned long key then a string as data.
 * Strings are held using buffers created by
 * malloc on decode. Tombstones are encoded
 * by a zero length string.
 */
bool_t xdr_orset_item(XDR *xdr, void* v)
{
    if (xdr->x_op == XDR_DECODE) {
        struct orset* os_c;
        uint64_t k;
        char* i;

        /* For decoding we are passed the
         * OrSet to add decoded items to.
         */
        os_c = (struct orset*) v;

        /* Grab some memory to store the
         * string.
         * TODO: Try instead of allocating
         * our own memory if you pass NULL
         * into xdr_string() I believe it
         * allocates memory for you.
         */
        if ((i = malloc(1024)) == NULL)
            printf("Malloc():\n");

        /* The key */
        if (!xdr_uint64_t(xdr, &k))
            printf("xdr_u_long():\n");

        if (orset_is_tombstone(k)) {
            free(i);

            /* The Tombstone value */
            if (!xdr_uint64_t(xdr, (uint64_t*) &i))
                printf("xdr_u_long():\n");

        } else {
            /* The String value */
            if (!xdr_string(xdr, &i, 1024))
                printf("xdr_string():\n");

            if (i[0] == '\0') {
                free(i);
                unordered_map_add(os_c->os_map, k, (void*) os_c);
                return 1;
            }
        }

        unordered_map_add(os_c->os_map, k, (void*) i);
        return 1;
    }

    if (xdr->x_op == XDR_ENCODE) {

        struct xdr_orset_item_cur* xic;

        /* For encoding we are passed an
         * 'iterator-like' structure
         */
        xic = (struct xdr_orset_item_cur*) v;

        /* Write the key */
        if (!xdr_uint64_t(xdr, &(xic->k)))
            printf("xdr_u_long():\n");

        if (orset_is_tombstone(xic->k)) {

            /* The Tombstone value */
            if (!xdr_uint64_t(xdr, (uint64_t*) &(xic->i)))
                printf("xdr_u_long():\n");

        } else if (orset_is_rockstone(xic->os, xic->i)) {

            /* Write the string item */
            if (!xdr_string(xdr, &NULL_STRING, 1024))
                printf("xdr_string():\n");

        } else {

            /* Write the string item */
            if (!xdr_string(xdr, (char**) &xic->i, 1024))
                printf("xdr_string():\n");
        }

        /* Advance the iterator */
        unordered_map_next(xic->os->os_map, &xic->k, &xic->i);
        return 1;
    }

    return 0;
}

/* OrSets encode to and decode from  an
 * unsigned long representing the node is
 * then an array of xdr_orset_items (see
 * above).
 */
bool_t xdr_orset(XDR *xdr, void* v)
{
    struct xdr_orset_item_cur xdr_os_i;
    struct orset* os_c;
    bool_t rc;
    size_t l;

    os_c = (struct orset*) v;

    /* 'v' points to an empty OrSet for
     * decoding
     */
    if (xdr->x_op == XDR_DECODE) {
        orset_create(os_c, 1);
    }

    /* 'v' points to a full OrSet for
     * encoding.
     */
    if (xdr->x_op == XDR_ENCODE) {
        /* For encoding we need to lock the
         * set to avoid invalid iterators.
         */
        sem_wait(&os_sem);

        /* Use an xdr_orset_item_cur as a
         * sort of iterator.
         */
        xdr_os_i.os = os_c;
        v = &xdr_os_i;

        /* Fill the size of an first iterator. */
        l = unordered_map_size(os_c->os_map);
        unordered_map_first(os_c->os_map, &xdr_os_i.k, &xdr_os_i.i);
    }

    /* We do not free the string in the OrSet
     * as ownership will be transferred to the
     * local OrSet and merge functions.
     * This may change in the future (copying is
     * cheap).
     */
    if (xdr->x_op == XDR_FREE) {
        unordered_map_free(((struct orset*) v)->os_map);
        return 1;
    }

    /* On the wire the node_id goes first then
     * the OrSet items (as an array).
     */
    rc = xdr_u_short(xdr,
            &os_c->os_node_id)
        && xdr_array(xdr,
            (char**) &v,
            (u_int*) &l,
            999999999,
            0,
            (xdrproc_t) xdr_orset_item);

    /* Unlock the OrSet. */
    if (xdr->x_op == XDR_ENCODE)
        sem_post(&os_sem);

    return rc;
}
