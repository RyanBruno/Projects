#ifndef ORSET_H
#define ORSET_H
/* The OrSet abstract data type. A type of
 * Conflict-free abstract data type, the
 * OrSet sits on top of an unordered_map
 * and provides additional functionality
 * that guarantees a merging set cannot
 * delete an item added by the local set
 * without knowledge of the addition and
 * vice-versa.
 */

#include "../unordered_map/unordered_map.h"
#define NODE_ID_OFFSET (sizeof(unsigned long) - sizeof(unsigned short)) * 8
/* The OrSet structure holds the underlying
 * unordered_map and some additional
 * information.
 */
struct orset {
    unordered_map os_map;       // The underlying unordered_map.
    unsigned short os_node_id;  // Used for the first two octets of
                                // every locally added key.
    unsigned long os_cur_id;    // Starts at node_id << NODE_ID_OFFSET
                                // and incremented every addition.
};

/* Initializes the OrSet setting os_cur_id
 * to 'node_id' << NODE_ID_OFFSET.
 */
void orset_create(struct orset* os, unsigned short node_id);

/* Locally adds and item (pointer) 'i' to
 * the set. This function returns the
 * unsigned long key now associated with
 * the item.
 * NOTE: The key's 2 most significant bytes
 * will always be the node_id.
 */
unsigned long orset_add(struct orset* os, void* i);

/* Replaces the pointer associated with 'k'
 * with a TOMBSTONE. During a merge
 * tombstones overwrite items.
 * SEE ALSO: orset_is_tombstone macro
 */
void orset_remove(struct orset* os, unsigned long k);

/* Gets the item (pointer) associated with
 * 'k'. Returns NULL the key was never added.
 * SEE ALSO orset_is_tombstone macro
 */
void* orset_get(struct orset* os, unsigned long k);

/* If an item from orset_get has been removed
 * with orset_remove this macro will expand
 * to true (1).
 */
#define orset_is_tombstone(os, v) (os == v)

/* Merges items in OrSet 'other' in to OrSet
 * 'os'. The merge algorithm goes as follows:
 * For each item in other...
 *  1. If item is a tombstone delete it in
 *     'os'
 *  2. If item is not in 'os' add it to 'os'
 */
void orset_merge(struct orset* os, struct orset* other);

#endif
