/* Some misc utility functions */
#include "orset/orset.h"

/* Prints all items in an OrSet */
void utils_print_set(struct orset* o);

/* Call this before calling
 * utils_print_stats.
 */
void utils_init();

/* Prints node_id,duration,items */
void utils_print_stats(struct orset* o, int node_id);
