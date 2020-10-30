/* Mimics a user for experiment purposes.
 */

/* The ratio of add to removes for each demo
 * operation.
 */
#define ADD_TO_REM_RATIO 3

/* The amount of seconds between each demo
 * operation. May also be represented as
 * seconds per operation.
 */
#define OPERATION_RATE 1

/* In order to start the bot this function
 * should be added to the 'threads' array. It
 * currently adds string from an array of
 * strings to the OrSet but it alternates
 * adding and removing items based on
 * ADD_TO_REM_RATIO.
 */
void* bot_thread_fn(void* v);

