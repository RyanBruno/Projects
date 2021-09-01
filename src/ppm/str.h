#include <string.h>

int str_equals(const char** a, const char** b)
{
    return !strcmp(*a, *b);
}

void str_move_mem(const char** cur, const char** other)
{
    *cur = *other;
}
