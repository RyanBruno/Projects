
int test_orset_baseline()
{
    struct orset os;
    unsigned long hi_k;
    unsigned long wrld_k;
    unsigned long fb_k;

    orset_create(&os, 1);

    hi_k = orset_add(&os, (void*) hi);
    wrld_k = orset_add(&os, (void*) wrld);
    fb_k = orset_add(&os, (void*) fb);

    if (
        hi != orset_get(&os, hi_k) ||
        wrld != orset_get(&os, wrld_k) ||
        fb != orset_get(&os, fb_k)
    ) return -1;

    return 0;
}

int test_orset_delete()
{
    struct orset os;
    unsigned long hi_k;
    unsigned long wrld_k;
    unsigned long fb_k;

    orset_create(&os, 1);

    hi_k = orset_add(&os, (void*) hi);
    wrld_k = orset_add(&os, (void*) wrld);
    fb_k = orset_add(&os, (void*) fb);

    orset_remove(&os, hi_k);

    if (!(orset_is_tombstone(&os, orset_get(&os, hi_k))))
        return -1;
    return 0;
}

int test_orset_large()
{
    struct orset os;
    unsigned long k;
    void* i;

    orset_create(&os, 1);

    for (int i = 0; i < 1000000; i++) {
        orset_add(&os, (void*) hi);
        orset_add(&os, (void*) wrld);
        orset_add(&os, (void*) fb);
    }

    unordered_map_reset(os.os_map);

    while (unordered_map_next(os.os_map, &k, &i)) {
        if (i == hi || i == wrld || i == fb)
            continue;
        return -1;
    }

    return 0;
}

int test_orset_merge()
{
    struct orset os;
    struct orset other;
    unsigned long k;
    void* i;
    long int t;

    orset_create(&os, 1);
    orset_create(&other, 2);

    /*
     * OS:
     *  1234: hi
     *  54321: <tomb>
     *  9876: fb
     * OTHER:
     *  1234: hi
     *  54321: wrld
     *  9876: <tomb>
     *  77777: fb
     * POST-MERGE
     *  hi
     *  <tomb>
     *  <tomb>
     *  fb
     */

    t = orset_add(&os, (void*) hi);
    unordered_map_add(other.os_map, t, (void*) hi);

    t = orset_add(&os, (void*) hi);
    orset_remove(&os, t);
    unordered_map_add(other.os_map, t, (void*) wrld);

    t = orset_add(&os, (void*) fb);
    orset_remove(&other, t);

    orset_add(&other, (void*) fb);

    orset_merge(&os, &other);

    t = 2;
    unordered_map_reset(os.os_map);

    while (unordered_map_next(os.os_map, &k, &i)) {
            
        if (orset_is_tombstone(&os, i))
            if (t-- > 0)
                continue;

        if (i == hi || i == fb)
            continue;

        return -1;
    }
    if (t != 0) return -1;

    return 0;
}

