/* Mimics a user for demo purposes.
 */
#include "demo.h"
#include "node.h"
#include <stdlib.h>
#include <unistd.h>

int items_collected;

/* Just some sample string so in a demo we
 * can easily identify corrupted data.
 */
char* strings[] = {
    "Hello",
    "World",
    "Ryan",
    "Bruno",
    "Python",
    "Shippensburg",
    "Red Raiders",
    "Dr. Armstrong",
    "Girard",
    "Briggs",
};

/* In order to start the demo this function
 * should be added to the threads array. It
 * currently adds string from an array of
 * strings to the OrSet but after 5
 * additions it alternates adding and
 * removing items.
 */
void* demo_thread_fn(void* v)
{
    int add = 1;
    int n = 0;

    /* Grace period */
    sleep(5);
    srand(node_id);

    for (;;) {

        if (add) {
            unsigned long k;
            char* s;

            s = strings[rand() % (sizeof(strings) / sizeof(char*))];
            sem_wait(&os_sem);
            k = orset_add(&os, s);
            sem_post(&os_sem);
        } else {
            size_t l;
            unsigned long k;
            void* i;

            sem_wait(&os_sem);
            l = unordered_map_size(os.os_map);
            l = rand() % l;
            unordered_map_reset(os.os_map);
            for (; l > 0; l--)
                unordered_map_next(os.os_map, &k, &i);
            orset_remove(&os, k);
            sem_post(&os_sem);
        }

        sleep(2);
        if (n > 2)
            add = !add;
        else
            n++;
    }
}

