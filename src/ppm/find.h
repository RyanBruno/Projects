
#define CCAT(x, y) x ## y
#define CCAT2(x, y) CCAT(x, y)

T2_TYPE*
CCAT2(find_, CCAT2(T2_PREFIX, T3)) (
        struct CCAT2(T1_PREFIX, T2)* um,
        T4* val)
{
    struct CCAT2(T1_PREFIX, CCAT2(T2_PREFIX, key_val))* kv;

    kv = CCAT2(T1_PREFIX, CCAT2(T2_PREFIX, first))(um);

    for (; kv != NULL;) {

        if (CCAT2(T4_PREFIX, equals)(&kv->val.T3, val))
            return &kv->val;

        kv = CCAT2(T1_PREFIX, CCAT2(T2_PREFIX, next))(um, kv);
    }

    return NULL;
}

#undef CCAT
#undef CCAT2
#undef T1
#undef T2
#undef T2_PREFIX
#undef T3
#undef T4
#undef T4_PREFIX
