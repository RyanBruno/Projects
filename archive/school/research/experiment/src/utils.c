/* Some misc utility functions */
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

static time_t utils_start_time;

/* Prints all items in an OrSet */
void utils_print_set(struct orset* o)
{
    unsigned long k;
    void* i;
    pid_t pid;
    int active_items = 0;
    int total_items = 0;

    /* Cheat and clear the screen */
    /*if (!(pid = fork()))
        execlp("clear", "clear", NULL);
    wait(NULL);*/

    printf("        == Current OrSet ==\n");

    if (!unordered_map_first(o->os_map, &k, &i))
        return;

    do {
        total_items++;
        if (orset_is_tombstone(k)) {
            printf("%16lx => (Tombstone)\n", k);
            continue;
        }
        if (orset_is_rockstone(o, i)) {
            printf("%16lx => (Rockstone)\n", k);
            continue;
        }
        active_items++;
        printf("%16lx => %s\n", k, i);
    } while (unordered_map_next(o->os_map, &k, &i));
    printf("            == stats ==\n");
    printf("      total_items %d\n", total_items);
    printf("     active_items %d\n", active_items);
    printf("             == END ==\n");
}

/* Call this before calling
 * utils_print_stats.
 */
void utils_init()
{
    utils_start_time = time(NULL);
}

/* Prints node_id,duration,items */
void utils_print_stats(struct orset* o, int node_id)
{
    printf("%d,%li,%lu\n", node_id, time(NULL) - utils_start_time,
            unordered_map_size(o->os_map));
}
