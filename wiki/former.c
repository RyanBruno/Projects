#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <microhttpd.h>

#define SERVERKEYFILE "server.key"
#define SERVERCERTFILE "server.pem"
#define PORT 8888
#define KEY "markdown"
#define POSTBUFFERSIZE  512
#define DEFAULT_NEXT "/"
#define UPLOAD_PATH "uploads"
#define UPLOAD_PATH_LEN 7
#define SIGNAL_PROGRAM "./signal.sh"
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
        size = 0;

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

struct connection_info_struct
{
    char *error_string; // Static string.
    const char* url;    // Handled by microhttpd
    char *data;         // Needs to be freed
    char *data_ptr;
    size_t data_cap;
    struct MHD_PostProcessor *postprocessor;
};

static int 
iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
    const char *filename, const char *content_type,
    const char *transfer_encoding, const char *data, 
    uint64_t off, size_t size)
{
    struct connection_info_struct *con_info = coninfo_cls;

    /* Check key */
    if (strcmp(key, KEY)) {
        con_info->error_string = "Unknown key\n";
        return MHD_NO;
    }

    /* alloc data */
    if (con_info->data == NULL) {

        con_info->data = malloc(4096);

        if (con_info->data == NULL) {
            con_info->error_string = "Out of memory\n";
            return MHD_NO;
        }

        con_info->data_ptr = con_info->data;
        con_info->data_cap = 4096;
    }

    /* Write data */
    if (size > 0) {

        /* Resize if needed */
        if (con_info->data_ptr - con_info->data + size > con_info->data_cap) {
            size_t ptr_offset;

            ptr_offset = con_info->data_ptr - con_info->data;
            con_info->data_cap *= 2;

            con_info->data = realloc(con_info->data, con_info->data_cap);

            if (con_info->data == NULL) {
                con_info->error_string = "Out of memory\n";
                return MHD_NO;
            }

            con_info->data_ptr = con_info->data + ptr_offset;
        }

        memcpy(con_info->data_ptr, data, size);
        con_info->data_ptr += size;
    }

    /* Success */
    return MHD_YES;
}

void request_completed(void *cls, struct MHD_Connection *connection, 
    void **con_cls,
    enum MHD_RequestTerminationCode toe)
{
    struct connection_info_struct *con_info = *con_cls;

    if (con_info == NULL) return;

    if (con_info->data != NULL) free(con_info->data);
    MHD_destroy_post_processor(con_info->postprocessor);        
    free(con_info);
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
    struct connection_info_struct *con_info = *con_cls;
    struct MHD_Response *response;
    const char* next;

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

    /* Connection setup */
    if (*con_cls == NULL) {

        if (url[0] != '/' && strstr(url, "../") != NULL) {
            return MHD_NO;
        }

        /* Create connection info struct */
        con_info = malloc(sizeof(struct connection_info_struct));
        if (con_info == NULL) return MHD_NO;

        /* Setup struct */
        con_info->url = url;
        con_info->error_string = NULL;
        con_info->data = NULL;

        /* Create postprocessor */
        con_info->postprocessor = MHD_create_post_processor(connection,
                POSTBUFFERSIZE, 
                (MHD_PostDataIterator) iterate_post, (void*) con_info);   
        if (con_info->postprocessor == NULL) {
            free(con_info); 
            return MHD_NO;
        }

        /* Setup compete */
        *con_cls = (void*) con_info; 
        return MHD_YES;
    }

    /* Continue POST processing */
    if (*upload_data_size != 0) {
        MHD_post_process (con_info->postprocessor, upload_data,   
            *upload_data_size);

        *upload_data_size = 0;

        /* Ask for more data */
        return MHD_YES;
    } 

    /* Handle Errors */
    if (con_info->error_string) {
        struct MHD_Response *response;

        response = MHD_create_response_from_buffer(
                            strlen(con_info->error_string),
                            (void*) con_info->error_string, 
                            MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header (response, "Content-Type", "text/plain");

        return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
    }

    /* TODO Error handling */
    /* Signal external program */
    int p[2];
    if (pipe(p) < -1) {
        // ERROR
    }

    if (SIGNAL_PROGRAM != NULL)
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

        execlp(SIGNAL_PROGRAM, SIGNAL_PROGRAM, url + 1,
                "7f84c21a0aa62acb5be82c4fd9936fa424d35979", NULL);
        break;
    case -1:
        // Error
        printf("Could not fork()\n");
        exit(-1);
        break;
    default:
        // Parent
        close(p[0]);
        for (size_t w = 0; ;) {
            size_t t;

            if (w >= con_info->data_ptr - con_info->data) break;

            t = write(p[1], con_info->data + w, (con_info->data_ptr - con_info->data) - w);
            w += t;
        }


        close(p[1]);
        wait(NULL);
        break;
    }
    /* END TODO Error handling */

    /* Redirect user */
    next = MHD_lookup_connection_value(connection,
                                           MHD_GET_ARGUMENT_KIND,
                                           NEXT_KEY);

    if (next == NULL) next = DEFAULT_NEXT;

    response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Location", next);

    return MHD_queue_response(connection, MHD_HTTP_FOUND, response);
}

int main(int argc, const char* argv[])
{
    // Start Http Server
    struct MHD_Daemon *daemon;
    char *key_pem;
    char *cert_pem;

    key_pem = load_file (SERVERKEYFILE);
    cert_pem = load_file (SERVERCERTFILE);
    if ((key_pem == NULL) || (cert_pem == NULL)) {
        printf ("The key/certificate files could not be read.\n");
        return 1;
    }
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD /*| MHD_USE_TLS*/,
                              PORT, NULL, NULL,
                        (MHD_AccessHandlerCallback) &answer_to_connection, NULL,
                        MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                        MHD_OPTION_HTTPS_MEM_KEY, key_pem,
                        MHD_OPTION_HTTPS_MEM_CERT, cert_pem,
                        MHD_OPTION_END);

    if (daemon == NULL) return 1;

    getchar();

    MHD_stop_daemon (daemon);
    free(key_pem);
    free(cert_pem);
    return 0;
}
