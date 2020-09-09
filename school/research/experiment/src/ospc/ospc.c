#include "ospc.h"
#include "../demo.h"

/* Wraps the orset into an ospc_context.
 */
void ospc_wrap(struct orset* os, struct ospc_context* oc)
{
    oc->oc_orset = os;
    oc->oc_latest_key_map = unordered_map_create();
}

/* The garbage collection algorithms goes as
 * follows. Before each merge, (1) ignore all
 * items and tombstone before that node's
 * latest key in the latest_key_map.
 * (2) Update latest key in latest_key_map
 * if necessary. If necessary removes all
 * tombstones in 'os' from other's node_id
 * before new_latest_key.
 */
void ospc_merge(struct ospc_context* oc, struct orset* other)
{
    unsigned long old_latest_key = 0;
    unsigned long new_latest_key = 0;
    unsigned long k;
    void* i;

    unordered_map_reset(other->os_map);

    while (unordered_map_next(other->os_map, &k, &i)) {
        unsigned long item_node;

        /* Get the originating node for this item */
        item_node = k >> NODE_ID_OFFSET;

        /* If we have already seen this item
         * directly from the node...
         */
        if ((unsigned long) unordered_map_get(oc->oc_latest_key_map, item_node) >= k)
            /* Ignore it */
            unordered_map_erase(other->os_map, k);


        /* If this is an item directly from the node
         * that created it.
         */
        if (other->os_node_id == item_node &&
            new_latest_key < k)
        {
            new_latest_key = k;
        }
    }

    old_latest_key = (unsigned long) unordered_map_get(oc->oc_latest_key_map, other->os_node_id);

    /* If we have actually seen a newer item
     * from this node.
     */
    if (old_latest_key < new_latest_key) {

        unordered_map_reset(oc->oc_orset->os_map);

        while (unordered_map_next(oc->oc_orset->os_map, &k, &i)) {
            unsigned long item_node;

            /* We on only collect tombstones */
            if (!orset_is_tombstone(oc->oc_orset, i))
                continue;

            /* Get the originating node for this item */
            item_node = k >> NODE_ID_OFFSET;

            /* Only applies to originating items */
            if (item_node != other->os_node_id)
                continue;

            /* All tombstones (< old_latest_key) are no
             * longer needed. All keys > old_latest_key
             * are needed for the merge.
             */
            if (k < old_latest_key) {
                unordered_map_erase(oc->oc_orset->os_map, k);
                items_collected++;
            }
        }

        /* If old_latest_key is zero, the node is new */
        if (old_latest_key)
            unordered_map_erase(oc->oc_latest_key_map, other->os_node_id);

        /* Update the latest key */
        unordered_map_add(oc->oc_latest_key_map, other->os_node_id, (void*) new_latest_key);
    }

    /* Finally merge the two sets */
    orset_merge(oc->oc_orset, other);
}