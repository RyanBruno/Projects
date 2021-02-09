#include <semaphore.h>

sem_t os_sem;
int test_orset_baseline()
{
    struct orset os;
    unsigned long hi_k;
    unsigned long wrld_k;
    unsigned long fb_k;

    if (sem_init(&os_sem, 0, 1)) {
        printf("sem_init():\n");
        exit(-1);
    }
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

    if (!(orset_is_rockstone(&os, orset_get(&os, hi_k))))
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

    unordered_map_first(os.os_map, &k, &i);

    do {
        if (i == hi || i == wrld || i == fb)
            continue;
        return -1;
    } while (unordered_map_next(os.os_map, &k, &i));

    return 0;
}

int test_orset_merge()
{
    struct orset os;
    struct orset other;
    unsigned long k;
    void* i;
    long int t;
    long int r;

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
    unordered_map_add(other.os_map, t, (void*) strdup(hi));

    t = orset_add(&os, (void*) hi);
    orset_remove(&os, t);
    unordered_map_add(other.os_map, t, (void*) strdup(wrld));

    t = orset_add(&os, (void*) strdup(fb));
    orset_remove(&other, t);

    orset_add(&other, (void*) strdup(fb));

    orset_merge(&os, &other);

    t = 2;
    r = 2;
    unordered_map_first(os.os_map, &k, &i);

    do {
        if (orset_is_rockstone(&os, i))
            if (r-- > 0)
                continue;
            
        if (orset_is_tombstone(k))
            if (t-- > 0)
                continue;

        if (!strcmp(i, hi) || !strcmp(i, fb))
            continue;

        return -1;
    } while (unordered_map_next(os.os_map, &k, &i));

    if (t != 0) return -1;
    if (r != 0) return -1;

    return 0;
}

