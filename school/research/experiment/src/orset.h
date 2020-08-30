#include "unordered_map.h"

struct orset {
    unordered_map os_map;
    unsigned short os_node_id;
    unsigned long os_cur_id;
};

void orset_create(struct orset* os, unsigned short node_id);

unsigned long orset_add(struct orset* os, void* i);

void orset_remove(struct orset* os, unsigned long k);

void* orset_get(struct orset* os, unsigned long k);

#define orset_is_tombstone(os, v) os == v

#define orset_collect unordered_map_erase

void orset_merge(struct orset* os, struct orset* other);
