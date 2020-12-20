



/* Kore JSON */

/* Initializes object and returns a handle
 * to the data or NULL on parse error or
 * out of memory.
 */
/*void* object_init(const char* data, size_t len)
{
    struct kore_json* json;

    json = malloc(sizeof(struct kore_json));
    kore_json_init(json, data, len);

    if (!kore_json_parse(json))
        return NULL;

    return json;
}

void object_free(void* obj)
{
    kore_json_cleanup(obj);
    free(obj);
}*/

void* object_add_object(void* obj, const char* name)
{
    return kore_json_create_object(obj, name)
}
void* object_add_array(void* obj, const char* name)
{
    return kore_json_create_array(obj, name, value);
}

void* object_add_string(void* obj, const char* name, const char* value)
{
    return kore_json_create_string(obj, name, value);
}

