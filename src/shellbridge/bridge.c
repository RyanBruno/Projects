#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <microhttpd.h>

#define SERVERKEYFILE "server.key"
#define SERVERCERTFILE "server.pem"
#define DEFAULT_PORT 8881
#define ARGV_KEY "argv"
#define SHELL_PROGRAM "/usr/bin/sh"
#define NEXT_KEY "next"

/* BEGIN: Taken from tutorial */
static long
get_file_size (const char *filename)
{
    FILE *fp;

    fp = fopen (filename, "rb");
    if (fp) {
        long size;

        if ((0 != fseek (fp, 0, SEEK_END)) || (-1 == (size = ftell (fp))))
        { size = 0; }

        fclose (fp);

        return size;
    }
    else return 0;
}
static char *
load_file (const char *filename)
{
    FILE *fp;
    char *buffer;
    long size;

    size = get_file_size (filename);
    if (0 == size)
        return NULL;

    fp = fopen (filename, "rb");
    if (!fp)
        return NULL;

    buffer = malloc (size + 1);
    if (!buffer) {
        fclose (fp);
        return NULL;
    }
    buffer[size] = '\0';

    if (size != (long) fread (buffer, 1, size, fp)) {
        free (buffer);
        buffer = NULL;
    }

    fclose (fp);
    return buffer;
}
/* END: Taken from tutorial */

void request_completed(void *cls, struct MHD_Connection *connection,
    void **con_cls,
    enum MHD_RequestTerminationCode toe)
{
    /* Unused args */
    (void) cls;
    (void) connection;
    (void) con_cls;
    (void) toe;
}


enum MHD_Result
get_argument_iterator(void *cls, enum MHD_ValueKind kind,
                     const char *key, const char *value)
{
    const char** conn_argv = ((const char**) cls);
    int i;

    if (kind != MHD_GET_ARGUMENT_KIND || strcmp(key, ARGV_KEY) || value == NULL)
    {
        return MHD_YES;
    }

    for (i = 0; conn_argv[i] != NULL; i++) { }

    conn_argv[i] = strdup(value);
    return MHD_YES;
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
    /* Unused args */
    (void) cls;
    (void) version;
    (void) upload_data;
    (void) upload_data_size;
    (void) con_cls;

    struct MHD_Response *response;
    const char* next;
    char* user;
    char* pass;

    /* POST only */
    if (strcmp(method, "POST")) {
        struct MHD_Response *response;
        const char* page = "<html><body>\
            What's your name, Sir?<br>\
            <form action=\"/input/test\" method=\"post\">\
            <textarea name=\"markdown\"></textarea>\
            <input type=\"submit\" value=\" Send \"></form>\
            </body></html>";

        response = MHD_create_response_from_buffer(strlen(page), (void*) page,
                            MHD_RESPMEM_PERSISTENT);

        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    user = MHD_basic_auth_get_username_password(connection,
                                                &pass);
    if (user == NULL || pass == NULL) {
        struct MHD_Response *response;

        response = MHD_create_response_from_buffer(0, NULL,
                            MHD_RESPMEM_PERSISTENT);

        return MHD_queue_basic_auth_fail_response(connection,
                                                "Please login.",
                                                response);
    }

    if (strcmp(user, "root") || strcmp(pass, "password")) {
        struct MHD_Response *response;

        response = MHD_create_response_from_buffer(0, NULL,
                            MHD_RESPMEM_PERSISTENT);

        return MHD_queue_basic_auth_fail_response(connection,
                                                "Incorrect password. Please Login.",
                                                response);
    }

    const char* conn_argv[1024];
    memset(conn_argv, '\0', 1024*sizeof(const char*));
    conn_argv[0] = url;
    char test[2048];
    memset(test, '\0', 2048);
    /*int conn_argc = */MHD_get_connection_values(connection,
                                              MHD_GET_ARGUMENT_KIND,
                                              (MHD_KeyValueIterator) &get_argument_iterator,
                                              conn_argv);
    /* TODO Error handling */
    /* Open a shell and pass commands */
    int p[2];
    if (pipe(p) < -1) {
        // ERROR
    }

    if (SHELL_PROGRAM != NULL)
    switch (vfork()) {
    case 0:
        close(p[1]);
        // Child
        if (close(0) < 0) {
            // Error
        }
        if (dup(p[0]) < 0) {
            // Error
        }

        execlp(SHELL_PROGRAM, SHELL_PROGRAM, NULL);
        break;
    case -1:
        // Error
        printf("Could not fork()\n");
        exit(-1);
        break;
    default:
        // Parent
        close(p[0]);
        for (int i = 0; conn_argv[i] != NULL; i++) {
            size_t s = strlen(conn_argv[i]);

            for (size_t w = 0; ;) {
                size_t t;

                if (w >= s) break;

                t = write(p[1], conn_argv[i] + w, s - w);
                w += t;
            }
            while (write(p[1], " ", 1) != 1) {}
        }

        close(p[1]);
        wait(NULL);
        break;
    }
    /* END TODO Error handling */

    /* Redirect user or return OK */
    next = MHD_lookup_connection_value(connection,
                                       MHD_GET_ARGUMENT_KIND,
                                       NEXT_KEY);

    if (next == NULL)
    {
        response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);

        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Location", next);

    return MHD_queue_response(connection, MHD_HTTP_FOUND, response);
}

int main(int argc, const char* argv[])
{
    struct MHD_Daemon *daemon;
    long port;
    char *key_pem;
    char *cert_pem;

    /* Load TLS files */
    key_pem = load_file(SERVERKEYFILE);
    cert_pem = load_file(SERVERCERTFILE);
    if (key_pem == NULL || cert_pem == NULL) {
        printf ("The key/certificate files could not be read.\n");
        return 1;
    }

    /* Parse port or use default */
    port = DEFAULT_PORT;
    if (argc > 1 && argv[1] != NULL)
        port = strtol(argv[1], NULL, 10);

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_TLS,
                              port, NULL, NULL,
                        (MHD_AccessHandlerCallback) &answer_to_connection, NULL,
                        MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                        MHD_OPTION_HTTPS_MEM_KEY, key_pem,
                        MHD_OPTION_HTTPS_MEM_CERT, cert_pem,
                        MHD_OPTION_END);

    if (daemon == NULL) return 1;

    //getchar();
    while(1) {}

    MHD_stop_daemon (daemon);
    free(key_pem);
    free(cert_pem);
    return 0;
}
