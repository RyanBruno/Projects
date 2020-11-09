#include "ospc.h"
#include <string.h>
#include <stdlib.h>

/* Wraps the orset into an ospc_context.
 */
void ospc_wrap(struct orset* os, struct ospc_context* oc)
{
    oc->oc_orset = os;
    oc->oc_latest_key_map = unordered_map_create();
    oc->oc_sent_map = unordered_map_create();
}

/* The garbage collection algorithms goes as
 * follows. Before each merge, (1) ignore all
 * items before that node's latest key in the
 * latest_key_map. (2) Update latest key in
 * latest_key_map if necessary. If necessary
 * removes all tombstones in 'os' from other's
 * node_id before old_latest_key.
 * Returns: The greatest items from 'other'
 * from other->os_node_id.
 */
uint64_t ospc_merge(struct ospc_context* oc, struct orset* other)
{
    uint64_t old_latest_key = 0;
    uint64_t new_latest_key = 0;
    uint64_t k;
    void* i;

    /* Ignore items already seen directly from each
     * node.
     */
    if (unordered_map_first(other->os_map, &k, &i)) {
        uint64_t erase = 0;
        do {
            uint64_t item_node;

            if (erase) {
                unordered_map_erase(other->os_map, erase);
                erase = 0;
            }

            /* Get the originating node for this item */
            item_node = k >> NODE_ID_OFFSET;

            /* If we have already seen this item
             * directly from the node or is one
             * of our items...
             */
            if (item_node == oc->oc_orset->os_node_id ||
                (uint64_t) unordered_map_get(oc->oc_latest_key_map, item_node) >= k)
            {
                /* Ignore it */
                if (!orset_is_tombstone(k) && !orset_is_rockstone(other, i))
                    free(i);
                erase = k;
            }

            /* If this is an item directly from the node
             * that created it.
             */
            if (other->os_node_id == item_node &&
                new_latest_key < k)
            {
                new_latest_key = k;
            }
        } while (unordered_map_next(other->os_map, &k, &i));

        if (erase) {
            unordered_map_erase(other->os_map, erase);
            erase = 0;
        }
    }

    /* Merge the two sets */
    orset_merge(oc->oc_orset, other);

    old_latest_key = (uint64_t) unordered_map_get(oc->oc_latest_key_map, other->os_node_id);

    /* If we have actually seen a newer item
     * from this node.
     */
    if (unordered_map_first(oc->oc_orset->os_map, &k, &i)) {
        uint64_t erase = 0;
        do {
            uint64_t item_node;

            if (erase) {
                unordered_map_erase(oc->oc_orset->os_map, erase);
                erase = 0;
            }

            /* We on only collect tombstones */
            if (!orset_is_tombstone(k) && !orset_is_rockstone(oc->oc_orset, i))
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
                erase = k;
            }
        } while (unordered_map_next(oc->oc_orset->os_map, &k, &i));

        if (erase) {
            unordered_map_erase(oc->oc_orset->os_map, erase);
            erase = 0;
        }
    }

    if (old_latest_key < new_latest_key) {
        /* If old_latest_key is zero, the node is new */
        if (old_latest_key)
            unordered_map_erase(oc->oc_latest_key_map, other->os_node_id);

        /* Update the latest key */
        unordered_map_add(oc->oc_latest_key_map, other->os_node_id, (void*) new_latest_key);
    }

    return new_latest_key;
}

/* Call this function just after a merge
 * with another node has been completed.
 * Return 1 if an update was made
 * ('latest_id' is greater then latest_id
 * in the in 'oc'), 0 if not.
 */
int ospc_touch(struct ospc_context* oc, uint64_t node_id,
                uint64_t latest_item)
{
    if ((uint64_t) unordered_map_get(oc->oc_sent_map, node_id) < latest_item) {
        unordered_map_erase(oc->oc_sent_map, node_id);
        unordered_map_add(oc->oc_sent_map, node_id, (void*) latest_item);
        return 1;
    }
    return 0;
}

/* The other half of the garbage collection
 * algorithm removes tombstones locally when
 * all nodes have been sent it directly.
 */
uint64_t ospc_collect(struct ospc_context* oc)
{
    uint64_t least_key;
    uint64_t k;
    uint64_t erase = 0;
    void* i;

    /* Set 'least_key' to uint64_t MAX_VALUE */
    memset(&least_key, '\xFF', sizeof(uint64_t));

    if (!unordered_map_first(oc->oc_sent_map, &k, &i))
        return 0;

    /* Find the least_key in the oc_sent_map. */
    do {
        if ((uint64_t) i < least_key)
            least_key = (uint64_t) i;
    } while (unordered_map_next(oc->oc_sent_map, &k, &i));

    /* Find some tombstones to collect. */
    if (!unordered_map_first(oc->oc_orset->os_map, &k, &i))
        return 0;

    do {
        uint64_t item_node;

        if (erase) {
            unordered_map_erase(oc->oc_orset->os_map, erase);
            erase = 0;
        }

        /* We only collect tombstones */
        if (!orset_is_tombstone(k) && !orset_is_rockstone(oc->oc_orset, i))
            continue;

        /* Get the originating node for this item */
        item_node = k >> NODE_ID_OFFSET;

        /* We only collect our items */
        if (item_node != oc->oc_orset->os_node_id)
            continue;

        /* Everyone has seen this tombestone
         * that we created. It is no longer
         * needed.
         */
        if (k <= least_key) {
            erase = k;
        }
    } while (unordered_map_next(oc->oc_orset->os_map, &k, &i));

    if (erase) {
        unordered_map_erase(oc->oc_orset->os_map, erase);
        erase = 0;
    }

    return least_key;
}
