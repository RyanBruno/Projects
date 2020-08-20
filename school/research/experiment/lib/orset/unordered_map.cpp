#include <unordered_map>

typedef void* unordered_map;

extern "C" unordered_map unordered_map_create()
{
    return (void*) new std::unordered_map<unsigned long, void*>();
}

extern "C" void unordered_map_add(unordered_map um, unsigned long k, void* i)
{
    ((std::unordered_map<unsigned long, void*>*) um)->insert(std::make_pair(k, i));
}

