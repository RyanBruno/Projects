#include "unordered_map.h"
#include <stdio.h>

typedef unordered_map orset;
unsigned long orset_add(orset os, void* i);
orset orset_create();
void orset_remove(orset os, unsigned long k);
#define orset_get unordered_map_get

const char* hi = "Hello ";
const char* wrld = "world ";
const char* fb = "foobar ";

int no_edge_cases()
{
    orset os;
    unsigned long hi_k;
    unsigned long wrld_k;
    unsigned long fb_k;

    os = orset_create();

    hi_k = orset_add(os, (void*) hi);
    wrld_k = orset_add(os, (void*) wrld);
    fb_k = orset_add(os, (void*) fb);

    if (
        hi != orset_get(os, hi_k) ||
        wrld != orset_get(os, wrld_k) ||
        fb != orset_get(os, fb_k)
    ) return -1;

    return 0;
}

void run_test(int(*fn)(), char* name)
{
    printf("[ ] Run \"%s\"...\n", name);

    if (!fn())
        printf("\033[F[P] Passed Test: %s\n", name);
    else
        printf("\033[F[F] Failed Test: %s\n", name);
}

int main()
{
    run_test(no_edge_cases, "Baseline Test");
}
