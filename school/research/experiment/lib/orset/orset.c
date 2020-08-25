#include "unordered_map.h"

typedef unordered_map orset;

#define orset_create unordered_map_create

/*
 * Adds a pointer 'i' to any item into the orset 'os'.
 * The underlying data structure stores that pointer as
 * a value of a random key. That key is then returned.
 * DO NOT try to add the orset into itself.
 */
unsigned long orset_add(orset os, void* i)
{
    unsigned long k = 1L; // Random long [0, LONG MAX)

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

    }
}

#define orset_collect unordered_map_erase

#define orset_get unordered_map_get
#define orset_is_tombstone(os, v) os == v

int main() { orset_get(orset_create(), 1); orset_collect(orset_create(), 1);}
