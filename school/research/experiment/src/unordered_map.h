struct unordered_map_pair {
    unsigned long k;
    void* i;
};

typedef void* unordered_map;

void* unordered_map_create();

void unordered_map_add(unordered_map um, unsigned long k, void* i);

void* unordered_map_get(unordered_map um, unsigned long k);

void unordered_map_reset(unordered_map um);

int unordered_map_next(unordered_map um, struct unordered_map_pair* ump);

void unordered_map_for_each(unordered_map um, void(*fn)(unsigned long, void*));

void unordered_map_erase(unordered_map um, unsigned long k);
