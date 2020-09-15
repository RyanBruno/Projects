#include "../src/utils.h"

int items_collected = 0;
int test_ospc_baseline()
{

    struct orset os;
    struct ospc_context oc;
    struct orset other;
    unsigned long k;
    void* i;
    long int t;

    orset_create(&os, 1);
    ospc_wrap(&os, &oc);
    orset_create(&other, 2);

    /*
     * OS: (1)
     *  0201: (tomb)
     *  0234: hi
     *  02321: wrld
     *  0276: fb
     * OTHER: (2)
     *  0234: (tomb)
     *  02321: (tomb) <----- LATEST seen
     *  0276: (tomb)
     * POST-MERGE
     *  hi
     *  <tomb>
     *  <tomb>
     */

    t = orset_add(&other, (void*) fb);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) fb);
    orset_remove(&os, t);

    t = orset_add(&other, (void*) hi);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(hi));

    t = orset_add(&other, (void*) wrld);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(wrld));
    unordered_map_add(oc.oc_latest_key_map, 2L, (void*) t);

    t = orset_add(&other, (void*) fb);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(fb));

    ospc_merge(&oc, &other);

    t = 1;
    unordered_map_reset(os.os_map);

    while (unordered_map_next(os.os_map, &k, &i)) {

        if (orset_is_tombstone(&os, i))
            if (t-- > 0)
                continue;

        if (!strcmp(i, hi) || !strcmp(i, wrld))
            continue;

        return -1;
    }
    if (t != 0) return -1;

    return 0;
}
