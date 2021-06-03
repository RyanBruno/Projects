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

/* Adds a pointer 'i' to any item into the orset 'os'.
 * The underlying data structure stores that pointer as
 * a value of a random key. That key is then returned.
 * DO NOT try to add the orset into itself.
 */
uint64_t orset_add(struct orset* os, void* i)
{
    uint64_t k;

    k = os->os_cur_id;
    os->os_cur_id += 2;
    unordered_map_add(os->os_map, k, i);

    return k;
}

/* orset_remove does two things. It removes
 * the item given by 'k' from the OrSet and
 * adds a tombstone pointing to the removed
 * item. Returns the id of the newly created
 * tombstone.
 */
uint64_t orset_remove(struct orset* os, uint64_t k)
{

    uint64_t kt;

    kt = os->os_cur_id + 1;
    os->os_cur_id += 2;

    unordered_map_add(os->os_map, kt, (void*) k);
    unordered_map_erase(os->os_map, k);
    unordered_map_add(os->os_map, k, (void*) os);

    return kt;
}

/*
 * Gets the pointer value associated with the
 * given key 'k'. Use orset_is_tombstone to check
 * if the item has is a tombstone
 */
void* orset_get(struct orset* os, uint64_t k)
{
    return unordered_map_get(os->os_map, k);
}

/* Merges orset 'other' into an orset 'os'.
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

    /* Start loop (return if empty) */
    if (!unordered_map_first(other->os_map, &k, &i))
        return;

    do {
        if (orset_is_rockstone(other, i)) continue;

        /* Step 1 */
        if (orset_is_tombstone(k)) {
            void* f;

            f = orset_get(os, (uint64_t) i);

            /* Free item if found and replace
             * with a rockstone.
             */
            if (f && !orset_is_rockstone(os, f)) {
                free(f);
                unordered_map_erase(os->os_map, (uint64_t) i);
                unordered_map_add(os->os_map, (uint64_t) i, (void*) os);
            }

            unordered_map_add(os->os_map, k, i);
            continue;
        }

        /* Step 2 */
        if (orset_get(os, k) == NULL) {
            unordered_map_add(os->os_map, k, i);
            continue;
        }
        free(i);

    } while (unordered_map_next(other->os_map, &k, &i));
}

