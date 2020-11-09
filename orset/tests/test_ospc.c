
int items_collected = 0;
int test_ospc_baseline()
{

    struct orset os;
    struct ospc_context oc;
    struct orset other;
    unsigned long k;
    void* i;
    long int t;
    long int r;

    orset_create(&os, 1);
    ospc_wrap(&os, &oc);
    orset_create(&other, 2);


    /*
     * OS: (1)
     * (rock)
     *  0201: (tomb)
     *  0234: hi
     *  02321: wrld
     *  0276: fb
     * OTHER: (2)
     * (rock)
     * (rock)
     * (rock)
     *  0234: (tomb)
     *  02321: (tomb) <----- LATEST seen
     *  0276: (tomb)
     * POST-MERGE
     *  hi
     *  <tomb>
     *  <tomb>
     *  (rock)
     *  (rock)
     *  (rock)
     */

    /* Trash */
    t = orset_add(&other, (void*) fb);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) fb);
    orset_remove(&os, t);

    /* Hello (in os, tomb in other) */
    t = orset_add(&other, (void*) hi);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(hi));

    /* World (in os, tomb in other) */
    t = orset_add(&other, (void*) wrld);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(wrld));
    unordered_map_add(oc.oc_latest_key_map, 2L, (void*) t);

    /* foobar (in os, tomb in other) */
    t = orset_add(&other, (void*) fb);
    orset_remove(&other, t);
    unordered_map_add(os.os_map, t, (void*) strdup(fb));

    //orset_merge(&os, &other);
    ospc_merge(&oc, &other);

    t = 3;
    r = 2;
    unordered_map_first(os.os_map, &k, &i);

    do {
        if (orset_is_rockstone(&os, i)) {
            if (r-- > 0)
                continue;
            return -1;
        }

        if (orset_is_tombstone(k)) {
            if (t-- > 0)
                continue;
            return -1;
        }

        if (!strcmp(hi, i))
            continue;

        return -1;
    } while (unordered_map_next(os.os_map, &k, &i));
    if (t != 0) return -1;
    if (r != 0) return -1;

    return 0;
}

int test_ospc_collect()
{

    struct orset os;
    struct ospc_context oc;
    unsigned long k;
    void* i;
    long int t;
    long int r;

    orset_create(&os, 1);
    ospc_wrap(&os, &oc);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);
    k = orset_remove(&os, k);
    ospc_touch(&oc, 0, k);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);

    k = orset_add(&os, (void*) hi);

    ospc_collect(&oc);

    t = 2;
    r = 2;
    unordered_map_first(os.os_map, &k, &i);

    do {
        if (orset_is_rockstone(&os, i)) {
            if (r-- > 0)
                continue;
            return -1;
        }

        if (orset_is_tombstone(k)) {
            if (t-- > 0)
                continue;
            return -1;
        }

        if (!strcmp(hi, i))
            continue;

        return -1;
    } while (unordered_map_next(os.os_map, &k, &i));
    if (t != 0) return -1;
    if (r != 0) return -1;
    return 0;
}
