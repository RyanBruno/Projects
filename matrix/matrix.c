#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <cjson/cJSON.h>

#include <microhttpd.h>

#define HOMESERVER_BASE_URL "http://127.0.0.1:8888"
#define PORT 8888

const char* user_g = "user";
const char* password_g = "pass";
const char* homeserver_g = ":rbruno.com";
const char* device_id_g;
const char* device_name_g;


/* Similar to read but (1) handles EAGAIN and
 * EINTR and (2) will always null-terminate.
 * This function decrements s to always make
 * sure there is room for a null-termination.
 */
char* read_some(int fd, char* b, size_t s)
{
    int r;

    for (s--;;) {

        errno = 0;
        if ((r = read(fd, b, s)) == 0)
            /* Either EOF or socket closed */
            break;

        /* Either again or error */
        if (r < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            /* Unknow error */
            return NULL;
        } 

        b += r;
        s -= r;
    }

    *b = '\0';
    return b;
}

int get_login(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    cJSON *arr;
    *res_r = cJSON_CreateObject();

    if (*res_r == NULL) goto error;
    if ((arr = cJSON_AddObjectToObject(*res_r, "flows")) == NULL) goto error;
    if (cJSON_AddStringToObject(arr, "type", "m.login.password") == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return 0;
}

int post_login(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    /* Parse JSON */
    cJSON *json = cJSON_Parse(post_str);
    cJSON *json_it;
    //cJSON *json_it = json->child;

    const char* type = NULL; // Password or token
    const char* user = NULL; // Depercated
    const char* medium = NULL; //Depercated
    const char* address = NULL; // Deperated
    const char* password = NULL; // Password
    const char* token = NULL; // Token
    const char* device_id = NULL; // If this does not correspond to a known client device, a new device will be created. The server will auto-generate a device_id if this is not specified.
    const char* initial_device_display_name = NULL; //  Ignored if device_id corresponds to a known device.

    cJSON *identifier = NULL;
    const char* id_type = NULL;
    const char* id_user = NULL;

    cJSON_ArrayForEach(json_it, json) {
        if (!strcmp(json_it->string, "identifier")) {
            identifier = json_it;
            continue;
        }

        if (!strcmp(json_it->string, "type")) {
            type = json_it->valuestring;
        } else if (!strcmp(json_it->string, "user")) {
            user = json_it->valuestring;
        } else if (!strcmp(json_it->string, "medium")) {
            medium = json_it->valuestring;
        } else if (!strcmp(json_it->string, "address")) {
            address = json_it->valuestring;
        } else if (!strcmp(json_it->string, "password")) {
            password = json_it->valuestring;
        } else if (!strcmp(json_it->string, "token")) {
            token = json_it->valuestring;
        } else if (!strcmp(json_it->string, "device_id")) {
            device_id = json_it->valuestring;
        } else if (!strcmp(json_it->string, "initial_device_display_name")) {
            initial_device_display_name = json_it->valuestring;
        } else {
            printf("Warning: Unknown attribute in request '%s'\n", json_it->string);
            continue;
        }
        if (!cJSON_IsString(json_it)) {
            printf("Attribute '%s' should be a string\n", json_it->string);
            return -1;
        }
    }

    cJSON_ArrayForEach(json_it, identifier) {
        if (!strcmp(json_it->string, "type")) {
            id_type = json_it->valuestring;
        } else if (!strcmp(json_it->string, "user")) {
            id_user = json_it->valuestring;
        } else {
            printf("Warning: Unknown attribute in request '%s'\n", json_it->string);
            continue;
        }
        if (!cJSON_IsString(json_it)) {
            printf("Attribute 'idenitifier.%s' should be a string\n", json_it->string);
            return -1;
        }
    }

    /*printf("type: %s\n", type);
    printf("user: %s\n", user);
    printf("medium: %s\n", medium);
    printf("address: %s\n", address);
    printf("password: %s\n", password);
    printf("token: %s\n", token);
    printf("device_id: %s\n", device_id);
    printf("initial_device_display_name: %s\n", initial_device_display_name);

    printf("id_type: %s\n", id_type);
    printf("id_user: %s\n", id_user);*/
    const char* fixed_user = id_user;

    if (fixed_user == NULL) {
        if ((fixed_user = user) == NULL) {
            // Error
            printf("no user\n");
            fixed_user = "wrong";
        }
        // TODO fix fully qualified users
    }

    if (strcmp(fixed_user, user_g)) {
        // Unknown user
        printf("unknown user\n");
    }

    if (password == NULL) {
        // Error
        printf("no password\n");
        password = "wrong";
    }

    if (strcmp(password, password_g)) {
        // Incorrect password
        printf("incorrect password\n");
    }

    if (device_id != NULL && (device_id_g == NULL || strcmp(device_id, device_id_g))) {
        device_id_g = device_id;
        device_name_g = initial_device_display_name;
        if (device_name_g == NULL)
            device_name_g = "Default device name";
    }

    /* TODO check database for login */
    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;
    char user_id[1024];
    user_id[0] = '@';
    strncpy(user_id + 1, fixed_user, 1023);
    strncat(user_id, homeserver_g, 1023);

    if (cJSON_AddStringToObject(*res_r, "user_id", user_id) == NULL) goto error;
    if (cJSON_AddStringToObject(*res_r, "access_token", "abc1234") == NULL) goto error;
    if (device_id)
        if (cJSON_AddStringToObject(*res_r, "device_id", device_id) == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");
    return -1;
}

int get_client(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    cJSON *arr;

    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;
    if ((arr = cJSON_AddObjectToObject(*res_r, "m.homeserver")) == NULL) goto error;
    if (cJSON_AddStringToObject(arr, "base_url", HOMESERVER_BASE_URL) == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return 0;

}

int get_capabilities(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    cJSON *caps;
    cJSON *cur;

    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;
    if ((caps = cJSON_AddObjectToObject(*res_r, "capabilities")) == NULL) goto error;
    if ((cur = cJSON_AddObjectToObject(caps, "m.change_password")) == NULL) goto error;
    if (cJSON_AddBoolToObject(cur, "enabled", 0) == NULL) goto error;
    if ((cur = cJSON_AddObjectToObject(caps, "m.room_versions")) == NULL) goto error;
    if (cJSON_AddStringToObject(cur, "default", "1") == NULL) goto error;
    if ((cur = cJSON_AddObjectToObject(cur, "available")) == NULL) goto error;
    if (cJSON_AddStringToObject(cur, "1", "stable") == NULL) goto error;
    if (cJSON_AddStringToObject(cur, "2", "stable") == NULL) goto error;
    if (cJSON_AddStringToObject(cur, "3", "stable") == NULL) goto error;
    if (cJSON_AddStringToObject(cur, "test-version", "unstable") == NULL) goto error;
    if ((cur = cJSON_AddObjectToObject(caps, "com.example.custom.ratelimit")) == NULL) goto error;
    if (cJSON_AddNumberToObject(cur, "max_requests_per_hour", 600) == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return 0;
}
const char* sync_str = "{"
"\"next_batch\": \"s72595_4483_1934\","
"\"presence\": {"
"\"events\": ["
"{"
"\"content\": {"
"\"avatar_url\": \"mxc://localhost:wefuiwegh8742w\","
"\"last_active_ago\": 2478593,"
"\"presence\": \"online\","
"\"currently_active\": false,"
"\"status_msg\": \"Making cupcakes\""
"},"
"\"type\": \"m.presence\","
"\"sender\": \"@example:localhost\""
"}"
"]"
"},"
"\"account_data\": {"
"\"events\": ["
"{"
"\"type\": \"org.example.custom.config\","
"\"content\": {"
"\"custom_config_key\": \"custom_config_value\""
"}"
"}"
"]"
"},"
"\"rooms\": {"
"\"join\": {"
"\"!726s6s6q:example.com\": {"
"\"summary\": {"
"\"m.heroes\": ["
"\"@alice:example.com\","
"\"@bob:example.com\""
"],"
"\"m.joined_member_count\": 2,"
"\"m.invited_member_count\": 0"
"},"
"\"state\": {"
"\"events\": ["
"{"
"\"content\": {"
"\"membership\": \"join\","
"\"avatar_url\": \"mxc://example.org/SEsfnsuifSDFSSEF\","
"\"displayname\": \"Alice Margatroid\""
"},"
"\"type\": \"m.room.member\","
"\"event_id\": \"$143273582443PhrSn:example.org\","
"\"room_id\": \"!726s6s6q:example.com\","
"\"sender\": \"@example:example.org\","
"\"origin_server_ts\": 1432735824653,"
"\"unsigned\": {"
"              \"age\": 1234"
"                                },"
"        \"state_key\": \"@alice:example.org\""
"                        }"
"]"
"},"
"\"timeline\": {"
"\"events\": ["
"{"
"\"content\": {"
"\"membership\": \"join\","
"\"avatar_url\": \"mxc://example.org/SEsfnsuifSDFSSEF\","
"\"displayname\": \"Alice Margatroid\""
"},"
"\"type\": \"m.room.member\","
"\"event_id\": \"$143273582443PhrSn:example.org\","
"\"room_id\": \"!726s6s6q:example.com\","
"\"sender\": \"@example:example.org\","
"\"origin_server_ts\": 1432735824653,"
"  \"unsigned\": {"
"                      \"age\": 1234"
"                                        },"
"                \"state_key\": \"@alice:example.org\""
"                                },"
"{"
"\"content\": {"
"\"body\": \"This is an example text message\","
"\"msgtype\": \"m.text\","
"\"format\": \"org.matrix.custom.html\","
"\"formatted_body\": \"<b>This is an example text message</b>\""
"},"
"\"type\": \"m.room.message\","
"\"event_id\": \"$143273582443PhrSn:example.org\","
"\"room_id\": \"!726s6s6q:example.com\","
"\"sender\": \"@example:example.org\","
"\"origin_server_ts\": 1432735824653,"
"\"unsigned\": {"
"                  \"age\": 1234"
"                                    }"
"}"
"],"
"\"limited\": true,"
"\"prev_batch\": \"t34-23535_0_0\""
"},"
"\"ephemeral\": {"
"\"events\": ["
"{"
"\"content\": {"
"\"user_ids\": ["
"\"@alice:matrix.org\","
"\"@bob:example.com\""
"]"
"},"
"\"type\": \"m.typing\","
"\"room_id\": \"!jEsUZKDJdhlrceRyVU:example.org\""
"}"
"]"
"},"
"\"account_data\": {"
"\"events\": ["
"{"
"\"content\": {"
"\"tags\": {"
"\"u.work\": {"
"\"order\": 0.9"
"                      }"
"}"
"},"
"\"type\": \"m.tag\""
"},"
"{"
"\"type\": \"org.example.custom.room.config\","
"\"content\": {"
"\"custom_config_key\": \"custom_config_value\""
"}"
"}"
"]"
"}"
"}"
"},"
"\"invite\": {"
"\"!696r7674:example.com\": {"
"\"invite_state\": {"
"\"events\": ["
"{"
"\"sender\": \"@alice:example.com\","
"\"type\": \"m.room.name\","
"\"state_key\": \"\","
"\"content\": {"
"\"name\": \"My Cool Room Name\""
"}"
"},"
"{"
"\"sender\": \"@alice:example.com\","
"\"type\": \"m.room.member\","
"\"state_key\": \"@bob:example.com\","
"\"content\": {"
"\"membership\": \"invite\""
"}"
"}"
"]"
"}"
"}"
"},"
"\"leave\": {}"
"}"
"}";

int get_sync(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    for (const char* p = *get_params; p != NULL; p = *(++get_params))
    {
        printf("Param: %s\n", p);
    }
    *res_r = cJSON_Parse(sync_str);
    if (*res_r == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return -1;

}

cJSON* filter = NULL;
int post_user_filter(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    filter = cJSON_Parse(post_str);
    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;

    if (cJSON_AddStringToObject(*res_r, "filter_id", "1234567") == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return -1;

}

int get_user_filter(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    *res_r = filter;
    if (*res_r == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return -1;

}

int get_presence(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;

    if (cJSON_AddStringToObject(*res_r, "presence", "online") == NULL) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return -1;

}

int put_presence(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{

    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;

    return 0;

error:
    printf("Unknown error\n");

    return -1;
}

int get_versions(const char* path, const char** get_params, const char* post_str, cJSON** res_r)
{
    const char* versions[] = {
        "r0.0.1",
        "r0.1.0",
        "r0.2.0",
        "r0.3.0",
        "r0.4.0",
        "r0.5.0",
        "r0.6.0"
    };
    cJSON *arr;

    *res_r = cJSON_CreateObject();
    if (*res_r == NULL) goto error;
    if ((arr = cJSON_CreateStringArray(versions, 7)) == NULL) goto error;
    if (!cJSON_AddItemToObject(*res_r, "versions", arr)) goto error;

    return 0;
error:
    printf("Unknown error\n");

    return -1;
}

enum cli_type {
    TYPE_CLI,
    TYPE_WEB
};

int cli(int argc, const char* argv[],
        const char** get_params,
        const char* post_str,
        cJSON** res_r)
{
    char buf[1024]; // TODO allow larger sizes.
    *res_r = NULL;

    if (argc < 3) {
        printf("Usage: matrix [method] [path]\n");
        return 0;
    }
    printf("argc: %d\n", argc);
    printf("path: %s\n", argv[2]);

    if (!strcmp("OPTIONS", argv[1])) {
        printf("method: OPTIONS(%s)\n", argv[1]);
        *res_r = NULL;
        return 0;
    } else if (!strcmp("/_matrix/client/r0/login", argv[2])) {
        printf("path: /_matrix/client/r0/login(%s)\n", argv[2]);
        if (!strcmp("POST", argv[1])) {
            printf("method: POST(%s)\n", argv[1]);
            post_login(argv[2], get_params, post_str, res_r);
        } else if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_login(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
    } else if (!strcmp("/_matrix/client/versions", argv[2])) {
        printf("path: /_matrix/client/versions(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_versions(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
    } else if (!strcmp("/.well-known/matrix/client", argv[2])) {
        printf("path: /.well-known/matrix/client(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_client(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
    } else if (!strcmp("/_matrix/client/r0/capabilities", argv[2])) {
        printf("path: /_matrix/client/r0/capabilities(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_capabilities(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
        /* Bookmark 1 */
    } else if (!strcmp("/_matrix/client/r0/sync", argv[2])) {
        printf("path: /_matrix/client/r0/sync(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_sync(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
    } else if (!strncmp("/_matrix/client/r0/presence/", argv[2], 28)) {
        printf("path: /_matrix/client/r0/presence(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_presence(argv[2], get_params, post_str, res_r);
        } else if(!strcmp("PUT", argv[1])) {
            printf("method: PUT(%s)\n", argv[1]);
            put_presence(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
        /* Bookmark 2 */
    } else if (!strncmp("/_matrix/client/r0/user/", argv[2], 24)) {
        printf("path: /_matrix/client/r0/user(%s)\n", argv[2]);
        if(!strcmp("GET", argv[1])) {
            printf("method: GET(%s)\n", argv[1]);
            get_user_filter(argv[2], get_params, post_str, res_r);
        } else if(!strcmp("POST", argv[1])) {
            printf("method: POST(%s)\n", argv[1]);
            post_user_filter(argv[2], get_params, post_str, res_r);
        } else {
            // Error
        }
    }

    if (*res_r == NULL) {
        *res_r = cJSON_CreateObject();
        printf("Unknown path\n");
        //return -1;
    }
    return 0;
}

struct get_params
{
    const char* get_params[21];
    size_t cur;
};

int get_iterator(void *cls,
                enum MHD_ValueKind kind,
                const char *key,
                const char *value)
{

    struct get_params* get_params;

    get_params = (struct get_params*) cls;
    printf("%s -> %s\n", key, value);

    get_params->get_params[get_params->cur++] = key;
    get_params->get_params[get_params->cur++] = value;
    return MHD_YES;
}

struct connection_info_struct
{
    char *data;
    size_t size;
    size_t offset;
};

int answer_to_connection (void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
    static int aptr;
    struct MHD_Response *response;
    int ret;

    struct connection_info_struct** cis = (struct connection_info_struct**) con_cls;
  
    /*if (0 != strcmp (method, "GET"))
        return MHD_NO;*/              /* unexpected method */


    if (*cis == NULL) {
        *cis = malloc(sizeof(struct connection_info_struct));
        (*cis)->data = NULL;
        (*cis)->size = 0;
        (*cis)->offset = 0;
        return MHD_YES;
    }

    if (*upload_data_size > 0) {
        if ((*cis)->data == NULL) {
            (*cis)->data = malloc(4096);
            (*cis)->size = 4095;
            (*cis)->offset = 0;
        }
        printf("DATA: %s\n", strndup(upload_data, *upload_data_size));
        if ((*cis)->offset + *upload_data_size > (*cis)->size) {
            printf("Out of room!\n");
            return MHD_NO;
        }

        memcpy((*cis)->data + (*cis)->offset, upload_data, *upload_data_size);
        (*cis)->offset += *upload_data_size;

        *upload_data_size = 0;
        return MHD_YES;
    }

    if ((*cis)->data != NULL) {
        (*cis)->data[(*cis)->offset] = '\0';
    } else if (!strcmp(method, "POST")) {
        /* POST w/ no data = bad */
        return MHD_NO;
    }

    cJSON* res;
    int rc;
    const char* argv[] = {
        "cli",
        method,
        url
    };
    int get_params_n;
    struct get_params get_params;
    get_params.cur = 0;

    get_params_n = MHD_get_connection_values(connection,
                    MHD_GET_ARGUMENT_KIND,
                    NULL,
                    NULL);

    if (get_params_n > 0) {
        if (get_params_n > 10) {
            // ERROR
        }

        MHD_get_connection_values(connection,
                    MHD_GET_ARGUMENT_KIND,
                    (MHD_KeyValueIterator) &get_iterator,
                    &get_params);
    }
    get_params.get_params[get_params.cur] = NULL;

    rc = cli(3, argv,
            get_params.get_params,
            (*cis)->data,
            &res);
    if (rc < 0) return MHD_NO;

    if (res != NULL) {
        const char* res_str = cJSON_Print(res);
        //printf("\nResponse: %s\n", res_str); // TODO free printed string

        response = MHD_create_response_from_buffer (strlen (res_str),
            (void*) res_str, MHD_RESPMEM_PERSISTENT);
    } else {
        response = MHD_create_response_from_buffer (0,
            NULL, MHD_RESPMEM_PERSISTENT);
    }
    MHD_add_response_header(response, "Content-Type", "application/json");
    /* CORS */
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Authorization");
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

    //MHD_destroy_post_processor((*cis)->postprocessor);
    //free(*cis);
    *cis = NULL;                  /* reset when done */
    return ret;
}

/* Broke but whatever */
int parse_gets(struct get_params* get_params,
        char* url)
{
    const char* token;
    token = strtok(url, "?=&");

    do {
        token = strtok(NULL, "=&");

        get_params->get_params[get_params->cur++] = token;

    } while (token != NULL);
    return 0;
}


int main(int argc, const char* argv[])
{
    int rc;
    cJSON* res;
    if (argc > 2) {
        char buf[1024];
        struct get_params get_params;
        get_params.cur = 0;

        /* TODO Parse get params */
        parse_gets(&get_params, (char*) argv[2]);

        /* Read Post data */
        read_some(0, buf, 1024);

        rc = cli(argc, argv, get_params.get_params, buf, &res);

        if (rc < 0)
            return rc;

        printf("\nResponse: %s\n", cJSON_Print(res)); // TODO free printed string
        return 0;
    }

    // Start Http Server
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                        (MHD_AccessHandlerCallback) &answer_to_connection, NULL,
                        MHD_OPTION_END);
    if (NULL == daemon) return 1;

    getchar();

    MHD_stop_daemon (daemon);
    return 0;
}
