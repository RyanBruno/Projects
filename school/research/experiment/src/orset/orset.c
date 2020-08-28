#include "orset.h"
/* For random */
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int rfd = -1;

orset orset_create()
{
    if (rfd == -1)
        rfd = open("/dev/random", O_RDONLY);

    return unordered_map_create();
}

/*
 * Adds a pointer 'i' to any item into the orset 'os'.
 * The underlying data structure stores that pointer as
 * a value of a random key. That key is then returned.
 * DO NOT try to add the orset into itself.
 */
unsigned long orset_add(orset os, void* i)
{
    unsigned long k; // Random long [0, LONG MAX)

    for (int p = 0; p < sizeof(k);)
        p += read(rfd, &k + p, sizeof(k) - p);

    unordered_map_add((unordered_map) os, k, i);

    return k;
}

/*
 * Replaces the item with a tombstone signifying
 * that the item has been removed.
 * DOES NOT free the pointer associated with 'k'.
 */
void orset_remove(orset os, unsigned long k)
{
    unordered_map_add((unordered_map) os, k, os);
}

void orset_merge(orset local, orset rmt)
{
    struct unordered_map_pair ump;

    unordered_map_reset(rmt);

    while (unordered_map_next(rmt, &ump)) {

        if (orset_is_tombstone(rmt, ump.i)) {
            orset_remove(local, ump.k);
            continue;
        }

        if (orset_get(local, ump.k) == NULL) {
            unordered_map_add(local, ump.k, ump.v);
            continue;
        }
    }
}

#define orset_collect unordered_map_erase

#define orset_get unordered_map_get
#define orset_is_tombstone(os, v) os == v
