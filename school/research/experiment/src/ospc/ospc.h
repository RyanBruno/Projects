/* OrSet Passive Collection (OSPC).
 * A conservative garbage collection for
 * OrSets in a network. This library expects
 * the operational ordering, item
 * origination node tag, and node_id given
 * by orset.h. This API used the decorator
 * pattern by wrapping around the orset in
 * question to provide extra functionality.
 * After a ospc_context is wrapped around an
 * OrSet use ospc_merge instead of
 * orset_merge.
 */
#ifndef OSPC_H
#define OSPC_H
#include "../orset/orset.h"

/* An OSPC conext keeps track of which items
 * have been sent from other nodes. This
 * information is used by and updated by
 * oscp_merge.
 */
struct ospc_context {
    struct orset* oc_orset;
    unordered_map oc_latest_key_map;
    unordered_map oc_sent_map;
};

/* Initializes ospc_context 'oc'
 */
void ospc_wrap(struct orset* os, struct ospc_context* oc);

/* The garbage collection algorithms goes as
 * follows. Before each merge, (1) ignore all
 * items and tombstone before that node's
 * latest key in the latest_key_map.
 * (2) Update latest key in latest_key_map
 * if necessary. If necessary removes all
 * tombstones in 'os' from other's node_id
 * before new_latest_key.
 * Returns: The greatest items from 'other'
 * from other->os_node_id.
 */
unsigned long ospc_merge(struct ospc_context* oc, struct orset* other);

/* Call this function just after a merge
 * with another node has been completed.
 * Return 1 if an update was made
 * ('greatest_id' is greater then greatest_id
 * in the in 'oc'), 0 if not.
 */
int ospc_touch(struct ospc_context* oc, unsigned long node_id, unsigned long item_id);

/* The other half of the garbage collection
 * algorithm removes tombstones locally when
 * all nodes have been sent it directly.
 */
void ospc_collect(struct ospc_context* oc);

#endif
