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
 */
void ospc_merge(struct ospc_context* oc, struct orset* other);

#endif
