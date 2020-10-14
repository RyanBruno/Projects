/* Mimics a user for experiment purposes.
 */
#include "bot.h"
#include "node.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>

extern time_t DURATION;
time_t bot_start_time;

#include "bot-data.h"

/* In order to start the demo this function
 * should be added to the threads array. It
 * currently adds string from an array of
 * strings to the OrSet but after 5
 * additions it alternates adding and
 * removing items.
 */
void* bot_thread_fn(void* v)
{
    unsigned char add = 1;
    int n = 0;

    /* Set the start time. */
    bot_start_time = time(NULL);

    for (;; add++) {

        /* This logic respects the DURATION experiment
         * variable. It waits 60 seconds with not more
         * additions or deletions then exit()s.
         */
        if (time(NULL) - bot_start_time > DURATION) {
            sleep(60);
            exit(0);
        }

        /* Operational pause. */
        sleep(OPERATION_RATE);

        if (add % ADD_TO_REM_RATIO != 0) {
            /* ADD */
            unsigned long k;
            char* s;

            /* Pick a string. */
            s = strings[rand() % (sizeof(strings) / sizeof(char*))];

            /* Add it to the set. */
            sem_wait(&os_sem);
            orset_add(&os, strdup(s));
            sem_post(&os_sem);

        } else {
            /* REMOVE */
            size_t l;
            unsigned long k;
            void* i;

            sem_wait(&os_sem);

            /* Pick a random element index. */
            l = unordered_map_size(os.os_map);
            l = rand() % l;

            /* Scan to that index. */
            if (!unordered_map_first(os.os_map, &k, &i))
                continue;
            for (; l > 1; l--)
                unordered_map_next(os.os_map, &k, &i);

            /* Either remove that one or the nearest
             * non-tombstone/non-rockstone.
             */
            do {
                if (!orset_is_tombstone(k) && !orset_is_rockstone(&os, i)) {
                    /* Free it first. */
                    free(orset_get(&os, k));

                    /* Remove from set. */
                    orset_remove(&os, k);
                    break;
                }
                /* Keep going if tombstone or rockstone */
            } while (unordered_map_next(os.os_map, &k, &i));
                
            sem_post(&os_sem);
        }
    }
}

