#include <unordered_map>
#include <stdexcept>

typename std::unordered_map<unsigned long, void*>::iterator unordered_map_it;

struct unordered_map_pair {
    unsigned long k;
    void* i;
};

typedef void* unordered_map;

extern "C" unordered_map unordered_map_create()
{
    return (void*) new std::unordered_map<unsigned long, void*>();
}

extern "C" void unordered_map_add(unordered_map um, unsigned long k, void* i)
{
    ((std::unordered_map<unsigned long, void*>*) um)->insert(std::make_pair(k, i));
}

extern "C" void* unordered_map_get(unordered_map um, unsigned long k)
{
    try {
        return ((std::unordered_map<unsigned long, void*>*) um)->at(k);
    } catch (const std::out_of_range& e) {
        return NULL;
    }
}

extern "C" void unordered_map_reset(unordered_map um)
{
    unordered_map_it = ((std::unordered_map<unsigned long, void*>*) um)->begin();
}

extern "C" int unordered_map_next(unordered_map um, struct unordered_map_pair* ump)
{
    if (unordered_map_it == ((std::unordered_map<unsigned long, void*>*) um)->end()) return 0;

    ump->k = unordered_map_it->first;
    ump->i = unordered_map_it->second;

    ++unordered_map_it;
    return 1;
}

extern "C" void unordered_map_for_each(unordered_map um, void(*fn)(unsigned long, void*))
{
    auto um_c = (std::unordered_map<unsigned long, void*>*) um;

    for (auto it = um_c->begin(); it != um_c->end(); ++it)
        fn(it->first, it->second);
}

extern "C" void unordered_map_erase(unordered_map um, unsigned long k)
{
    ((std::unordered_map<unsigned long, void*>*) um)->erase(k);
}
