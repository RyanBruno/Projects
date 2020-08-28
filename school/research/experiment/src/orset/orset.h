#include "unordered_map.h"

typedef unordered_map orset;

unsigned long orset_add(orset os, void* i);
orset orset_create();
void orset_remove(orset os, unsigned long k);
#define orset_get unordered_map_get
