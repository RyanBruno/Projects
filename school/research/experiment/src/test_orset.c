#include <stdio.h>
#include "orset.h"

const char* hi = "Hello ";
const char* wrld = "world ";
const char* fb = "foobar ";

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
    struct unordered_map_pair ump;

    orset_create(&os, 1);

    for (int i = 0; i < 1000000; i++) {
        orset_add(&os, (void*) hi);
        orset_add(&os, (void*) wrld);
        orset_add(&os, (void*) fb);
    }

    unordered_map_reset(os.os_map);

    while (unordered_map_next(os.os_map, &ump)) {
        if (ump.i == hi || ump.i == wrld || ump.i == fb)
            continue;
        return -1;
    }

    return 0;
}

int test_orset_merge()
{
    struct orset os;
    struct orset other;
    struct unordered_map_pair ump;
    long int k;

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

    k = orset_add(&os, (void*) hi);
    unordered_map_add(os.os_map, k, (void*) hi);

    k = orset_add(&os, (void*) hi);
    orset_remove(&os, k);
    unordered_map_add(other.os_map, k, (void*) wrld);

    k = orset_add(&os, (void*) fb);
    orset_remove(&other, k);

    orset_add(&other, (void*) fb);

    orset_merge(&os, &other);

    k = 2;
    unordered_map_reset(os.os_map);

    while (unordered_map_next(os.os_map, &ump)) {
            
        if (orset_is_tombstone(&os, ump.i))
            if (k-- > 0)
                continue;

        if (ump.i == hi || ump.i == fb)
            continue;

        return -1;
    }
    if (k != 0) return -1;

    return 0;

}

void run_test(int(*fn)(), char* name)
{
    printf("[ ] Running \"%s\"...\n", name);

    if (!fn())
        printf("\033[F[P] Passed Test: %s\n", name);
    else
        printf("\033[F[F] Failed Test: %s\n", name);
}

int main()
{
    run_test(test_orset_baseline, "[orset.c] Baseline Test");
    run_test(test_orset_delete, "[orset.c] Delete Test");
    run_test(test_orset_large, "[orset.c] Large Dataset Test");
    run_test(test_orset_merge, "[orset.c] Merge Test");
}
