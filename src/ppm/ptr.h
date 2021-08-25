
#define T0_CCAT(x) CCAT2(T0_PREFIX, x)
#define T01_CCAT(x) CCAT2(T01_PREFIX, x)

typedef struct {
    T0* ptr;
} T0_CCAT(ptr);

T0_CCAT(ptr)
T0_CCAT(ptr_move) (T0_CCAT(ptr)* cur)
{
    T0_CCAT(ptr) other;

    other.ptr = cur->ptr;

    return other;
}

T0_CCAT(ptr)
T0_CCAT(ptr_construct) (T0_CCAT(ptr)* ptr, void* p)
{
    ptr->ptr = (T0*) p;
}

#ifdef T01
T01_CCAT(ptr)
T0_CCAT(ptr_morph) (T0_CCAT(ptr)* ptr, void* p)
{
    T01_CCAT(ptr) other;

    other.ptr = (T01*) p;

    return other;
}
#endif

#undef T0_CCAT
#undef T0_PREFIX
#undef T0
