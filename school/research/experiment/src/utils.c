/* Some misc utility functions */
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "demo.h"


/* Prints all items in an OrSet */
void print_set(struct orset* o)
{
    unsigned long k;
    void* i;
    pid_t pid;
    int active_items = 0;

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
        active_items++;
        printf("%16lx => %s\n", k, i);
    }
    printf("           == stats ==\n");

    printf("     active_items %d\n", active_items);
    printf("  items_collected %d\n", items_collected);
    printf("             == END ==\n");
}
