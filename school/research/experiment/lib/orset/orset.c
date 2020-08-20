#include "unordered_map.h"

typedef unordered_map orset;

#define orset_create unordered_map_create

void orset_add(orset os, void* i)
{
    unordered_map_add((unordered_map) os, /* random k */1L, i);
}

int main() { orset_create(); }
