#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/uio.h>

#include <arpa/inet.h>
#include <netdb.h>
//#include <pthread.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define BACKLOG  10
#define MAX_RCPT 100
#define MAX_PATH_LEN 256
#define RAND_LEN 64
#define PATH_EXTRA RAND_LEN + 5
#define PAGE_SIZE 4096
#define STR_OVERHEAD_SIZE (sizeof(struct string) - sizeof(void*))
/* Coming soon */
#define authorized 0

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

/* Random */
int rfd;

char* greeting = "220 flappy.rbruno.com\r\n";
int greeting_len;

const char* DATA_END_TOKEN = "\r\n.\r\n";

/* String Abstraction */
struct string {
    size_t str_size;
    void* str_str;
};

struct str_iterator {
    struct string* str;
    size_t str_offset;
};

/* Calls realloc on str and updates str->str_size.
 * Returns null on error or a resized string on
 * success.
 */
struct string* str_resize(struct string* str, size_t new_size)
{
    if ((str = realloc(str, new_size)) == NULL) return NULL;
    str->str_size = new_size - STR_OVERHEAD_SIZE;
    return str;
}

/* I/O Abstraction */
struct input_iterator {
    int* it_f;
    struct input_functions* it_if;
    int it_i;
    char it_buf[PAGE_SIZE - sizeof(int) - (2*sizeof(void*))];
};

struct input_functions {
    ssize_t(*if_read_a)(int*, void*, size_t);
    ssize_t(*if_write_a)(int*, const void*, size_t);
    void(*if_cleanup_a)(struct input_iterator *it);
};

ssize_t read_r(int* f, void* b, size_t s) { read(*f, b, s); }
ssize_t write_w(int* f, const void* b, size_t s) { write(*f, b, s); }
void input_cleanup_f(struct input_iterator* it) { close(*it->it_f); }
struct input_functions if_berkeley = {
    .if_read_a = read_r,
    .if_write_a = write_w,
    .if_cleanup_a = input_cleanup_f,

};

ssize_t read_s(int* f, void* b, size_t s) { SSL_read((SSL*) f, b, s); }
ssize_t write_s(int* f, const void* b, size_t s) { SSL_write((SSL*) f, b, s); }
void input_cleanup_s(struct input_iterator* it)
{
    SSL* ssl = (SSL*) it->it_f;
    SSL_shutdown(ssl);
    SSL_free(ssl);
    EVP_cleanup();
    close(SSL_get_fd(ssl));
}
struct input_functions if_ssl = {
    .if_read_a = read_s,
    .if_write_a = write_s,
    .if_cleanup_a = input_cleanup_s,
};

int input_starttls(struct input_iterator* it)
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX *ctx;
    SSL *ssl;

    if ((ctx = SSL_CTX_new(SSLv23_server_method())) == NULL) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_chain_file(ctx, "../fullchain.pem") <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "../key.pem", SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, *it->it_f);
    it->it_f = (int*) ssl;
    it->it_if = &if_ssl;

    /* Wait for client to initiate a handshake */
    return SSL_accept(ssl);
}

/* Similar to read but (1) handles EAGAIN and
 * EINTR and (2) will always null-terminate.
 * This function decrements s to always make
 * sure there is room for a null-termination.
 */
#define read_some(it, b, s) _read_some(it->it_f, it->it_if->if_read_a, b, s)
char* _read_some(int* f, ssize_t(*read_a)(int*, void*, size_t), char* b, size_t s)
{
    int r;

    for (s--;;) {

        errno = 0;
        if ((r = read_a(f, b, s)) == 0)
            /* Either EOF or socket closed */
            break;

        /* Either again or error */
        if (r < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            break;
        } 

        b += r;
        s -= r;
        break;
    }

    *b = '\0';
    return b;
}

/* Returns the next char in it, reading if
 * necessary.
 */
char input_next(struct input_iterator* it)
{
    if (it->it_buf[it->it_i] != '\0')
        /* Next char is available */ 
        return it->it_buf[it->it_i++];

    if (read_some(it, it->it_buf, sizeof(it->it_buf)) == it->it_buf)
        /* EOF has been reached */
        return '\0';

    it->it_i = 0;
    return it->it_buf[it->it_i++];
}

/* Calls input_next until c is found.
 */
void input_find(struct input_iterator* it, char c)
{
    for (char s = input_next(it); s != c && s != '\0'; s = input_next(it)) { }
}

/* Loads a string in it->it_buf ending with char c.
 * The returned int is the starting offset and
 * it->it_i is set to one past c.
 */
int input_until(struct input_iterator* it, char c)
{
    int i;

    for (i = it->it_i;; it->it_i++) {

        if (it->it_buf[it->it_i] == '\0') {
            if (i != 0) {
                it->it_i = i - it->it_i;
                /* memcpy i to 0 */
                for (int j = 0;;) {
                    it->it_buf[j++] = it->it_buf[i];
                    if (it->it_buf[i++] == '\0') break;
                }
                i = 0;
            }

            if (it->it_i >= sizeof(it->it_buf))
                return -1;

            read_some(it, it->it_buf + it->it_i, sizeof(it->it_buf) - it->it_i);
        }

        if (it->it_buf[it->it_i] == c) break;
    }
    it->it_i++;
    return i;
}

/* Ensures all s chars of b are written to fd.
 * Returns -1 on error 0 on success. If -1 is
 * returned assume fd is closed*.
 * * By closed I mean do not try again;
 *   close(fd) should still be called. *
 */
#define write_all(it, b, s) _write_all(it->it_f, it->it_if->if_write_a, b, s)
#define write_all_f(f, b, s) _write_all(&f, write_w, b, s)
int _write_all(int* f, ssize_t(*write_a)(int*, const void*, size_t), const char* b, size_t s)
{
    int w;

    for (;;) {
        w = write_a(f, b, s);

        if (w < 1) {
            if (errno == EAGAIN || errno == EINTR) continue;
            return -1;
        }

        b += w;
        s -= w;

        if (s == 0) break;
    }
    return 0;
}

/* SMTP structs and helper functions */
struct smtp_transaction {
    char tx_r[RAND_LEN];                /* Random bytes generated at construction */
    char tx_spool_path[10 + RAND_LEN];
    struct str_iterator tx_str_head;    /* A String to hold all the rev-paths, fwd-paths... */
    size_t tx_rev_path;                 /* The reverse path (sender). */
    size_t tx_fwd_path[MAX_RCPT];       /* The forward paths (recipients). */
    int tx_fwd_path_len;
    struct iovec tx_rmt_path[MAX_RCPT]; /* The remote paths (non-local recipients). */
    int tx_rmt_path_len;
};

/* Pre-ESMTP a SMTP session was state-less
 * except for a transaction.
 */
struct smtp_context {
    struct smtp_transaction smtp_tx;
};

struct smtp_response {
    char* res;
    size_t res_len;
};

char* smtp_off_to_str(struct smtp_context* sc, size_t o)
{
    return (char*) &sc->smtp_tx.tx_str_head.str->str_str + o;
}

void smtp_cleanup(struct input_iterator* it) { it->it_if->if_cleanup_a(it); exit(0); }

void smtp_abort_cleanup(struct input_iterator* it)
{
    write_all(it, "451 Requested action aborted: local error in processing\r\n", 57);
    smtp_cleanup(it);
}

size_t save_path(struct str_iterator* str_it, struct input_iterator* it, int n)
{
    char c;
    size_t s_it_r;

    /* Find the start */
    for (c = input_next(it); c == '<' || c == ' '; c = input_next(it)) { }

    for (s_it_r = str_it->str_offset; c != '\r' && c != '>'; c = input_next(it), str_it->str_offset++) {

        if (n < 0 || c == '\0') smtp_abort_cleanup(it);

        /* Forward slashes are illegal */
        if (c == '/')
            continue;

        /* Resize */
        if (str_it->str_offset >= str_it->str->str_size)
            if ((str_it->str = str_resize(str_it->str, str_it->str->str_size * 1.5)) == NULL)
                smtp_abort_cleanup(it);
    
        ((char*) &str_it->str->str_str)[str_it->str_offset] = c;
    }

    ((char*) &str_it->str->str_str)[str_it->str_offset++] = '\0';

    return s_it_r;
}

/* SMTP commands */
void unknown_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    res_r->res = "500 Syntax error\r\n";
    res_r->res_len = 18;
}

void helo_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    res_r->res = "250 SMTPd greats you\r\n";
    res_r->res_len = 22;
}

void ehlo_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    res_r->res = "250-SMTPd greats you\r\n250-STARTTLS\r\n250 PIPELINING\r\n";
    res_r->res_len = 52;
}

void starttls_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    if (write_all(it, "220 GO ahead\r\n", 14) < 0)
        smtp_cleanup(it);

    if (input_starttls(it) <= 0)
        smtp_abort_cleanup(it);

    res_r->res_len = 0;
}

void mail_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    /* Ordering */
    if (sc->smtp_tx.tx_rev_path != 0)
        return unknown_command(sc, it, res_r);

    /* Setup a transaction */
    sc->smtp_tx.tx_str_head.str = str_resize(NULL, PAGE_SIZE);

    /* Save reverse path */
    sc->smtp_tx.tx_rev_path = save_path(&sc->smtp_tx.tx_str_head, it, MAX_PATH_LEN);
    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len].iov_base = smtp_off_to_str(sc, sc->smtp_tx.tx_rev_path);
    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len++].iov_len = sc->smtp_tx.tx_str_head.str_offset - 1;

    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len].iov_base = ",";
    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len++].iov_len = 1;

    /* Setup Random */
    for (char* e = sc->smtp_tx.tx_r; e - sc->smtp_tx.tx_r < sizeof(sc->smtp_tx.tx_r) - 1;)
        e += read(rfd, e, sizeof(sc->smtp_tx.tx_r) - (e - sc->smtp_tx.tx_r));

    /* Encode Random */
    for (int i = 0; i < sizeof(sc->smtp_tx.tx_r) - 1; i++)
        sc->smtp_tx.tx_r[i] = (abs(sc->smtp_tx.tx_r[i]) % 26) + 97;
    sc->smtp_tx.tx_r[sizeof(sc->smtp_tx.tx_r) - 1] = '\0';

    res_r->res = "250 OK\r\n";
    res_r->res_len = 8;
}

void rcpt_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    size_t o;
    struct stat st;

    /* Ordering */
    if (sc->smtp_tx.tx_str_head.str_offset == 0)
        return unknown_command(sc, it, res_r);

    /* Too many rcpts */
    if (sc->smtp_tx.tx_fwd_path_len >= MAX_RCPT) // TODO Error message
        return unknown_command(sc, it, res_r);

    /* Save forward_path */
    o = sc->smtp_tx.tx_str_head.str_offset;
    sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len] = save_path(&sc->smtp_tx.tx_str_head, it, MAX_PATH_LEN);

    if (stat(smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len]), &st) == 0) {

        /* Print rest of /tmp/ path */
        memcpy(smtp_off_to_str(sc, sc->smtp_tx.tx_str_head.str_offset) - 1, "/tmp/", 5);
        memcpy(smtp_off_to_str(sc, sc->smtp_tx.tx_str_head.str_offset) + 4, sc->smtp_tx.tx_r, sizeof(sc->smtp_tx.tx_r));
        sc->smtp_tx.tx_str_head.str_offset += sizeof(sc->smtp_tx.tx_r) + 5;

        /* Push forward rcpt */
        sc->smtp_tx.tx_fwd_path_len++;

        res_r->res = "250 OK\r\n";
        res_r->res_len = 8;
        return;
    }

    if (!authorized) {
        /* Reset head */
        sc->smtp_tx.tx_str_head.str_offset = o;

        res_r->res = "551 User not local\r\n";
        res_r->res_len = 20;
        return;
    }

    /* Need to forward */

    /* Push to rmt_path instead for scatter/gather */
    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len].iov_base = smtp_off_to_str(sc, o);
    sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len++].iov_len = sc->smtp_tx.tx_str_head.str_offset - o;
    smtp_off_to_str(sc, sc->smtp_tx.tx_str_head.str_offset)[-1] = ',';

    if (sc->smtp_tx.tx_rmt_path_len == 3) {
        /* First remote path setup */
        /* Ensure there is enough room */
        if (sc->smtp_tx.tx_str_head.str_offset >= sc->smtp_tx.tx_str_head.str->str_size - RAND_LEN - 10)
            if ((sc->smtp_tx.tx_str_head.str = str_resize(sc->smtp_tx.tx_str_head.str, sc->smtp_tx.tx_str_head.str->str_size * 1.5)) == NULL)
                smtp_abort_cleanup(it);

        memcpy(smtp_off_to_str(sc, sc->smtp_tx.tx_str_head.str_offset), "spool/tmp/", 10);
        memcpy(smtp_off_to_str(sc, sc->smtp_tx.tx_str_head.str_offset) + 10, sc->smtp_tx.tx_r, RAND_LEN);
        sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len++] = sc->smtp_tx.tx_str_head.str_offset;
        sc->smtp_tx.tx_str_head.str_offset += RAND_LEN + 10;
    }

    res_r->res = "251 User not local; will forward\r\n";
    res_r->res_len = 34;
}

void data_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    int ffds[MAX_RCPT];

    /* Ordering */
    if (sc->smtp_tx.tx_fwd_path_len < 1)
        return unknown_command(sc, it, res_r);

    /* Open tmp files */
    for (int i = 0; i < sc->smtp_tx.tx_fwd_path_len; i++) {

        /* Open the temp file */
        if ((ffds[i] = open(smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i]), O_CREAT | O_EXCL | O_WRONLY, 0644)) < 0)
            smtp_abort_cleanup(it);


        if (!strncmp(smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i]), "spool/tmp/", 10)) {
            sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len].iov_base = "\r\n";
            sc->smtp_tx.tx_rmt_path[sc->smtp_tx.tx_rmt_path_len++].iov_len = 2;
            writev(ffds[i], sc->smtp_tx.tx_rmt_path, sc->smtp_tx.tx_rmt_path_len); // TODO error checking
        };

        ; // TODO SMTP headers

    }

    input_find(it, '\n');
    if (write_all(it, "354 Start mail input; end with <CRLF>.<CRLF>\r\n", 46) < 0)
        smtp_cleanup(it);

    for (;;) {
        int i;

        if ((i = input_until(it, '\n')) < 0)
            smtp_abort_cleanup(it);

        if (!strcmp(".\r\n", it->it_buf + i)) break;

        /* Write to all files */
        for (int j = 0; j < sc->smtp_tx.tx_fwd_path_len; j++)
            if (write_all_f(ffds[j], it->it_buf + i, it->it_i - i) < 0)
                smtp_abort_cleanup(it);
    }
    it->it_buf[it->it_i] = '\n';

    /* Move all files from tmp to new */
    for (int i = 0; i < sc->smtp_tx.tx_fwd_path_len; i++) {
        char nb[MAX_PATH_LEN + PATH_EXTRA];

        /* Copy to nb */
        for (int j = 0;; j++) {
            if (smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i])[j] == '\0') { nb[j] = '\0'; break; }
            if (smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i])[j] == '/') { memcpy(nb + j, "/new/", 5); j += 4; continue; }
            nb[j] = smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i])[j];
        }

        /* Move file from tmp to new */
        if (rename(smtp_off_to_str(sc, sc->smtp_tx.tx_fwd_path[i]), nb) < 0)
            smtp_abort_cleanup(it); // TODO rollback half commits

        /* Cleanup */
        close(ffds[i]);
    }

    free(sc->smtp_tx.tx_str_head.str);
    memset(&sc->smtp_tx, '\0', sizeof(sc->smtp_tx));

    res_r->res = "250 will deliver\r\n";
    res_r->res_len = 18;
}

void rset_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    if (sc->smtp_tx.tx_str_head.str != NULL)
        free(sc->smtp_tx.tx_str_head.str);
    memset(&sc->smtp_tx, '\0', sizeof(sc->smtp_tx));

    res_r->res = "250 OK\r\n";
    res_r->res_len = 8;
}

void noop_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    res_r->res = "250 OK\r\n";
    res_r->res_len = 8;
}

void quit_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    write_all(it, "250 OK\r\n", 8);
    smtp_cleanup(it);
}

int stritmatch(char* str, struct input_iterator* it, int n)
{
    for (char c; n > 0; str++, n--) {
        c = input_next(it);

        if (toupper(c) != *str) return 0;
        if (*str == '\0') break;
    }
    return 1;
}

void(*command_mapper(struct input_iterator* it))(struct smtp_context*, struct input_iterator*, struct smtp_response*)
{
    switch (toupper(input_next(it))) {
    case 'H':
        if (stritmatch("ELO", it, 3))       return helo_command;
    case 'E':
        if (stritmatch("HLO", it, 3))       return ehlo_command;
    case 'S':
        if (stritmatch("TARTTLS", it, 7))   return starttls_command;
    case 'M':
        if (stritmatch("AIL FROM:", it, 9)) return mail_command;
    case 'R':
        switch (toupper(input_next(it))) {
        case 'C':
            if (stritmatch("PT TO:", it, 6)) return rcpt_command;
        case 'S':
            if (stritmatch("ET", it, 2))     return rset_command;
        }
    case 'D':
        if (stritmatch("ATA", it, 3))       return data_command;
    case 'N':
        if (stritmatch("OOP", it, 3))       return noop_command;
    case 'Q':
        if (stritmatch("UIT", it, 3))       return quit_command;
    }
    return unknown_command;
}

int smtp(int fd, struct sockaddr_in* pa)
{
    struct smtp_context sc;
    struct input_iterator it;

    memset(&sc.smtp_tx, '\0', sizeof(sc.smtp_tx));

    it.it_f = &fd;
    it.it_i = 0;
    it.it_buf[0] = '\0';
    it.it_if = &if_berkeley;

    /* Greeting */
    if (write_all((&it), greeting, greeting_len) < 0)
        smtp_cleanup(&it);
    
    for (;;) {
        struct smtp_response sr;

        command_mapper(&it)(&sc, &it, &sr);

        if (sr.res_len)
            if (write_all((&it), sr.res, sr.res_len) < 0)
                smtp_cleanup(&it);

        input_find(&it, '\n');
    }
}

int main(int argc, char **argv)
{
    printf("%d\n", sizeof(struct input_iterator));
    return -1;
    int afd;
    struct addrinfo *a;

    char* host = NULL;
    char* port = NULL;

    while (*++argv != NULL) {
        if (argv[0][0] != '-')
            goto help;

        switch(argv[0][1]) {
        case 'h':
            host = *++argv;
            break;
        case 'p':
            port = *++argv;
            break;
        default:
            goto help;
        }
    }

    if (host == NULL || port == NULL)
        goto help;

    /* Socket setup */
    if (getaddrinfo(host, port, &ai_hints, &a) != 0 ||
       (afd = socket(a->ai_family, a->ai_socktype, a->ai_protocol)) == -1 ||
        bind(afd, a->ai_addr, a->ai_addrlen) ||
        listen(afd, BACKLOG))
    {
        fprintf(stderr, "Could not open socket.\n");
        return -1;
    }

    freeaddrinfo(a);

    signal(SIGCHLD, SIG_IGN);

    rfd = open("/dev/urandom", O_RDONLY);
    greeting_len = strlen(greeting);

    do {
        int fd;
        struct sockaddr_in pa;
        socklen_t pa_size = sizeof(pa);

        errno = 0;
        if ((fd = accept(afd, (struct sockaddr*) &pa, &pa_size)) < 0)
            continue;

        if (fork() == 0) {
            smtp(fd, &pa);
            close(fd);
            exit(0);
        }

    } while (errno == 0 || errno == EINTR);

    return -1;

help:
    printf("smtpd -h <host> -p <port>\n");
    return -1;
}
