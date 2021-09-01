#include <stdlib.h>
#include <assert.h>

#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)
#define T1_CCAT(x) CCAT2(unordered_map_, CCAT2(T1_PREFIX, x))
#define T2_CCAT(x) CCAT2(unordered_map_, CCAT2(T2_PREFIX, x))

struct T2_CCAT(key_val) {
    T1 key;
    T2 val;
};

struct CCAT2(unordered_map_, T2_TYPE) {
    struct T2_CCAT(key_val)* data;
    size_t s;
    size_t c;
};

void T2_CCAT(construct)(struct CCAT2(unordered_map_, T2_TYPE)* um)
{
    um->data = NULL;
    um->s = 0;
    um->c = 0;
}

void T2_CCAT(insert)(struct CCAT2(unordered_map_, T2_TYPE)* um,
        T1* k,
        T2* v /* moved */)
{
    if (um->data == NULL || um->s >= um->c)
    {
        void* ptr;

        ptr = malloc((um->s + 5) * sizeof(struct T2_CCAT(key_val)));
        assert(ptr != NULL);

        um->data = ptr;
        um->c = um->s + 5;
    }

    CCAT2(T1_PREFIX, move_mem)(k, &um->data[um->s].key);
    CCAT2(T2_PREFIX, move_mem)(v, &um->data[um->s].val);
    um->s++;
}

void T2_CCAT(remove)(struct CCAT2(unordered_map_, T2_TYPE)* um, T1* k)
{
}

struct T2_CCAT(key_val)*
T2_CCAT(first)(struct CCAT2(unordered_map_, T2_TYPE)* um)
{
    if (um->s == 0) return NULL;

    return &um->data[0];
}

struct T2_CCAT(key_val)*
T2_CCAT(next)(struct CCAT2(unordered_map_, T2_TYPE)* um,
        struct T2_CCAT(key_val)* kv)
{
    for (int i = 0; i < um->s; i++) {
        struct T2_CCAT(key_val)* tmp;

        tmp = &um->data[i];

        if (CCAT2(T1_PREFIX, equals)(&tmp->key, &kv->key))
            return tmp;
    }

    return NULL;
}

T2* T2_CCAT(get)(struct CCAT2(unordered_map_, T2_TYPE)* um, T1* k)
{
    for (int i = 0; i < um->s; i++) {
        struct T2_CCAT(key_val)* kv;

        kv = &um->data[i];

        if (CCAT2(T1_PREFIX, equals)(&kv->key, k))
            return &kv->val;
    }

    return NULL;
}

#undef T2_CCAT
#undef T1_CCAT
#undef CCAT2
#undef CCAT
#undef T2_TYPE
#undef T2_PREFIX
#undef T2
#undef T1_PREFIX
#undef T1
