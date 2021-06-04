#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "../node.h"
#include "../xdr_orset/xdr_orset.h"

const char* hi = "Hello ";
const char* wrld = "world ";
const char* fb = "foobar ";

void run_test(int(*fn)(), char* name)
{
    printf("[ ] Running \"%s\"...\n", name);

    if (!fn())
        printf("\033[F[P] Passed Test: %s\n", name);
    else
        printf("\033[F[F] Failed Test: %s\n", name);
}


#define TESTS_INTERNAL
#include "test_orset.c"
#include "test_xdr_orset.c"
#include "test_ospc.c"

int main()
{
    run_test(test_ospc_baseline, "[ospc.c] Baseline Test");
    run_test(test_ospc_collect, "[ospc.c] Local Collection Test");
    run_test(test_orset_baseline, "[orset.c] Baseline Test");
    run_test(test_orset_delete, "[orset.c] Delete Test");
    run_test(test_orset_merge, "[orset.c] Merge Test");
    run_test(test_orset_large, "[orset.c] Large Dataset Test");
    run_test(test_xdr_orset_encode, "[xdr_orset.c] Encode Test");
}
