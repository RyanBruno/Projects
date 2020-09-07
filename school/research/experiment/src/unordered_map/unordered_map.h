/* A public interface for an unordered_map
 * implementation.
 */
#include <stddef.h>

/* The unordered_map handle is usually a
 * pointer to underlying unordered_map
 * implementation
 */
typedef void* unordered_map;

/* Initializes the unordered_map.
 * NOTE: Implementation handles allocations
 * so you should call unordered_map_free()
 * on the returned pointer when done.
 */
void* unordered_map_create();

/* Frees the unordered_map */
void unordered_map_free(unordered_map um);

/* Associates unsigned long 'k' with pointer
 * 'i' in the unordered_map 'um'. Users must
 * call unordered_map_reset before
 * unordered_map_next after this call
 */
void unordered_map_add(unordered_map um, unsigned long k, void* i);

/* Erases unsigned long 'k' and its associated
 * value from the unordered_map 'um'.
 */
void unordered_map_erase(unordered_map um, unsigned long k);

/* Gets the associated value (pointer) with
 * the unsigned long 'k'.
 */
void* unordered_map_get(unordered_map um, unsigned long k);

/* Used to start a loop across all items in
 * the unordered_map. Calling this first will
 * reset an internal variable to allow
 * unordered_map_next to return the first
 * item in the unordered_map, the next on the
 * next call ...
 */
void unordered_map_reset(unordered_map um);

/* After unordered_map_reset is called this
 * function returns the first item in the
 * unordered_map 'um' and returns it in 'k'
 * and 'i'. Will return 1 on success and 0
 * when the end has been reached.
 */
int unordered_map_next(unordered_map um, unsigned long* k, void** i);

/* Returns the number of elements in the
 * map.
 */
size_t unordered_map_size(unordered_map um);

