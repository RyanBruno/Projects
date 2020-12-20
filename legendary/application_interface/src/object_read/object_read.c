#include <stddef.h>
#include "object_read.h"

void object_read(const char* object, const char* pk)
{
    if (pk == NULL) {
        return object_read_all(object);
    } else {
        return object_read_one(object, pk);
    }
}
