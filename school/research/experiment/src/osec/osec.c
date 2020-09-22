#include "osec.h"
#include "../node.h"
#include <stdio.h>


void osec_eager_collect(struct ospc_context* oc, uint64_t latest_id)
{
    uint64_t k;
    void* i;
    uint64_t old_latest_id;
    uint64_t item_node;

    item_node = latest_id >> NODE_ID_OFFSET;

    old_latest_id = (uint64_t) unordered_map_get(oc->oc_latest_key_map, item_node);

    if (old_latest_id > latest_id) {
        printf("osec_eager_collect(): old > new %lx > %lx\n", old_latest_id, latest_id);
        return;
    }

    unordered_map_erase(oc->oc_latest_key_map, item_node);
    unordered_map_add(oc->oc_latest_key_map, item_node, (void*) latest_id);

    unordered_map_reset(oc->oc_orset->os_map);

    while (unordered_map_next(oc->oc_orset->os_map, &k, &i)) {
        uint64_t cur_item_node;

        /* We on only collect tombstones */
        if (!orset_is_tombstone(oc->oc_orset, i))
            continue;

        /* Get the originating node for this item */
        cur_item_node = k >> NODE_ID_OFFSET;

        /* Only applies to originating items */
        if (cur_item_node != item_node)
            continue;

        /* All tombstones (< old_latest_key) are no
         * longer needed. All keys > old_latest_key
         * are needed for the merge.
         */
        if (k < old_latest_id) {
            unordered_map_erase(oc->oc_orset->os_map, k);
            //items_collected++;
        }
    }

}
