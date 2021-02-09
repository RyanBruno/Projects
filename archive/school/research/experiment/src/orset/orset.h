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
#define node_t uint8_t
#define NODE_ID_OFFSET (sizeof(uint64_t) - sizeof(node_t)) * 8
/* The OrSet structure holds the underlying
 * unordered_map and some additional
 * information.
 */
struct orset {
    unordered_map os_map;       // The underlying unordered_map.
    unsigned short os_node_id;  // Used for the first two octets of
                                // every locally added key.
    uint64_t os_cur_id;         // Starts at node_id << NODE_ID_OFFSET
                                // and incremented every addition.
};

/* Initializes the OrSet setting os_cur_id
 * to 'node_id' << NODE_ID_OFFSET.
 */
void orset_create(struct orset* os, node_t node_id);

/* Locally adds and item (pointer) 'i' to
 * the set. This function returns the
 * uint64_t key now associated with the item.
 * NOTE: The key's 2 most significant bytes
 * will always be the node_id.
 */
uint64_t orset_add(struct orset* os, void* i);

/* orset_remove does two things. It removes
 * the item given by 'k' from the OrSet and
 * adds a tombstone pointing to the removed
 * item. Returns the id of the newly created
 * tombstone.
 */
uint64_t orset_remove(struct orset* os, uint64_t k);

/* Gets the item (pointer) associated with
 * 'k'. Returns NULL the key was never added.
 * SEE ALSO orset_is_tombstone macro
 */
void* orset_get(struct orset* os, uint64_t k);

/* Use this to tell if a key id is a tombstone
 * or not.
 */
#define orset_is_tombstone(v) ((v & 1) == 0)

/* Use this to tell if a key id is a rockstone
 * or not.
 */
#define orset_is_rockstone(o, v) (o == v)

/* Merges items in OrSet 'other' in to OrSet
 * 'os'. The merge algorithm goes as follows:
 * For each item in other...
 *  1. If item is a tombstone delete it in
 *     'os'
 *  2. If item is not in 'os' add it to 'os'
 */
void orset_merge(struct orset* os, struct orset* other);

#endif
