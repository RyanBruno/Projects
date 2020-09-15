/* A unordered_map implementation as
 * a proxy for the STL's unordered_map
 * with C bindings.
 */
#include <unordered_map>
#include <stdexcept>

/* The interal iterator needed for the
 * unordered_map_(reset,next) to work.
 */
static typename std::unordered_map<uint64_t, void*>::iterator unordered_map_it;

typedef void* unordered_map;

extern "C" unordered_map unordered_map_create()
{
    return (void*) new std::unordered_map<uint64_t, void*>();
}

extern "C" void unordered_map_free(unordered_map um)
{
    delete ((std::unordered_map<uint64_t, void*>*) um);
}

extern "C" void unordered_map_add(unordered_map um, uint64_t k, void* i)
{
    auto rc = ((std::unordered_map<uint64_t, void*>*) um)->insert(std::make_pair(k, i));
    if (!rc.second)
        printf("DID NOT INSERT %lu, %s\n", k, i);
}

extern "C" void* unordered_map_get(unordered_map um, uint64_t k)
{
    try {
        return ((std::unordered_map<uint64_t, void*>*) um)->at(k);
    } catch (const std::out_of_range& e) {
        return NULL;
    }
}

extern "C" void unordered_map_reset(unordered_map um)
{
    unordered_map_it = ((std::unordered_map<uint64_t, void*>*) um)->begin();
}

extern "C" int unordered_map_next(unordered_map um, uint64_t* k, void** i)
{
    if (unordered_map_it == ((std::unordered_map<uint64_t, void*>*) um)->end()) return 0;

    *k = unordered_map_it->first;
    *i = unordered_map_it->second;

    ++unordered_map_it;
    return 1;
}

extern "C" void unordered_map_erase(unordered_map um, uint64_t k)
{
    ((std::unordered_map<uint64_t, void*>*) um)->erase(k);
}

extern "C" size_t unordered_map_size(unordered_map um)
{
    return (size_t) ((std::unordered_map<uint64_t, void*>*) um)->size();
}
