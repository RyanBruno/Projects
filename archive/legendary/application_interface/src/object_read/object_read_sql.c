#include "object_read.h"

void* object_read_all(const char* object)
{
    void* object_array;
    void* object_item;
    unsigned int num_fields;
    MYSQL_ROW row;
    unsigned long* data_lengths;

    /* result = SELECT * FROM object; */

    object_array = object_add_array(NULL, NULL); // TODO NULL check

    num_fields = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result))) {
        data_lengths = mysql_fetch_lengths(result);

        object_item = object_create_object(object_array, NULL);

        for(unsigned int i = 0; i < num_fields; i++) {
            const char *str;

            if (!data_lengths[i])
                continue;

            str = strndup(row[i], (int) data_lengths[i]);

            object_add_string(object_item, "name", str);
        }
    }

    return object_array;
}

void object_read_one(const char* object, const char* pk)
{
    /* SELECT * FROM object WHERE pk = pk; */

}
