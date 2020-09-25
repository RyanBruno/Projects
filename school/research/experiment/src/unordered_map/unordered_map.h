/* A public interface for an unordered_map
 * implementation.
 */
#include <stddef.h>
#include <stdint.h>

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

/* Associates uint64_t 'k' with pointer 'i'
 * in the unordered_map 'um'. Users must
 * call unordered_map_reset before
 * unordered_map_next after this call
 */
void unordered_map_add(unordered_map um, uint64_t k, void* i);

/* Erases uint64_t 'k' and its associated
 * value from the unordered_map 'um'.
 */
void unordered_map_erase(unordered_map um, uint64_t k);

/* Gets the associated value (pointer) with
 * the uint64_t 'k'.
 */
void* unordered_map_get(unordered_map um, uint64_t k);

/* Used to start a loop across all items in
 * the unordered_map. Sets 'k' and 'i' to
 * the first key and value (respectively) in
 * 'um'. Return 1 on success and 0 if the map
 * is empty.
 */
int unordered_map_first(unordered_map um, uint64_t* k, void** i);

/* After unordered_map_first is called this
 * function can be used to get the next item
 * in the map 'um'. Will return 1 on success
 * and 0 when the end has been reached.
 */
int unordered_map_next(unordered_map um, uint64_t* k, void** i);

/* Returns the number of elements in the
 * map.
 */
size_t unordered_map_size(unordered_map um);

