/* Some misc utility functions */
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


/* Prints all items in an OrSet */
void print_set(struct orset* o)
{
    unsigned long k;
    void* i;
    pid_t pid;

    /* Cheat and clear the screen */
    if (!(pid = fork()))
        execlp("clear", "clear", NULL);
    wait(NULL);

    printf("        == Current OrSet ==\n");
    unordered_map_reset(o->os_map);
    while (unordered_map_next(o->os_map, &k, &i)) {
        if (orset_is_tombstone(o, i)) {
            printf("%16lx => (Tombstone)\n", k);
            continue;
        }
        printf("%16lx => %s\n", k, i);
        printf("%16lx => %d\n", k, i);
    }
    printf("              == END ==\n");
}
