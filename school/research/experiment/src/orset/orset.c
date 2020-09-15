#include "orset.h"
#include <stddef.h>
#include <stdlib.h>

/*
 * Initializes the orset pointed to by 'os'.
 */
void orset_create(struct orset* os, node_t node_id)
{
    os->os_map = unordered_map_create();
    os->os_node_id = node_id;
    os->os_cur_id = ((uint64_t) node_id << NODE_ID_OFFSET) + 1;
}

/*
 * Adds a pointer 'i' to any item into the orset 'os'.
 * The underlying data structure stores that pointer as
 * a value of a random key. That key is then returned.
 * DO NOT try to add the orset into itself.
 */
uint64_t orset_add(struct orset* os, void* i)
{
    uint64_t k;

    k = os->os_cur_id++;
    unordered_map_add(os->os_map, k, i);

    return k;
}

/*
 * Replaces the item with a tombstone signifying
 * that the item has been removed.
 * DOES NOT free the pointer associated with 'k'.
 */
void orset_remove(struct orset* os, uint64_t k)
{
    unordered_map_erase(os->os_map, k);
    unordered_map_add(os->os_map, k, (void*) os);
}

/*
 * Gets the pointer value associated with the
 * given key 'k'. Use orset_is_tombstone to check
 * if the item has been removed.
 */
void* orset_get(struct orset* os, uint64_t k)
{
    unordered_map_get(os->os_map, k);
}

/*
 * Merges orset 'other' into an orset 'os'.
 * The merge algorithm goes as follows:
 *  1. All tombstones in 'other' will be
 *     removed in 'os'.
 *  2. All item in 'other' not in 'os' will
 *     be added to 'os'.
 * NOTE: This function frees a token when it
 *       removed.
 */
void orset_merge(struct orset* os, struct orset* other)
{
    uint64_t k;
    void* i;

    unordered_map_reset(other->os_map);

    while (unordered_map_next(other->os_map, &k, &i)) {

        if (orset_is_tombstone(other, i)) {
            void* f;

            f = orset_get(os, k);

            /* Free all non-tombstones */
            if (!orset_is_tombstone(os, f))
                free(f);

            orset_remove(os, k);
            continue;
        }

        if (orset_get(os, k) == NULL) {
            unordered_map_add(os->os_map, k, i);
            continue;
        }
    }
}

