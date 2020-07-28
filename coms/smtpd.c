#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>
//#include <pthread.h>

#define BACKLOG  10
#define MAX_RCPT 100
#define MAX_PATH_LEN 256
#define RAND_LEN 64
#define PATH_EXTRA RAND_LEN + 5
#define DEFAULT_STR_SIZE 4096
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

char* greeting = "220 SMTP Service Ready\r\n";
int greeting_len;

char spool_path[10 + RAND_LEN];
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

struct string* str_resize(struct string* str, size_t new_size)
{
    if ((str = realloc(str, new_size)) == NULL) return NULL;
    str->str_size = new_size - STR_OVERHEAD_SIZE;
    return str;
}

/* I/O Abstraction */
struct input_iterator {
    int fd;
    int i;
    char input_buffer[1024];
};

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
            break;
        } 

        b += r;
        s -= r;
        break;
    }

    *b = '\0';
    return b;
}

/* Unwanted dep */
void cleanup(int fd) { close(fd); exit(0); }

char input_next(struct input_iterator* it)
{
    if (it->input_buffer[it->i] != '\0')
        return it->input_buffer[it->i++];

    if (read_some(it->fd, it->input_buffer, sizeof(it->input_buffer)) == it->input_buffer)
        cleanup(it->fd);

    it->i = 0;
    return it->input_buffer[it->i++];
}

void input_find(struct input_iterator* it, char c)
{
    for (char s = input_next(it); s != c; s = input_next(it)) { }
}

void write_all(int fd, const char* b, size_t s)
{
    int w;

    for (;;) {
        w = write(fd, b, s);

        if (w < 1) {
            if (errno == EAGAIN || errno == EINTR) continue;
            cleanup(fd);
        }

        b += w;
        s -= w;

        if (s == 0) return;
    }
}

/* SMTP structs and helper functions */
struct smtp_transaction {
    char tx_r[RAND_LEN];
    struct str_iterator tx_str_head;
    size_t tx_rev_path;
    size_t tx_fwd_path[MAX_RCPT];
    int tx_fwd_path_len;
    size_t tx_rmt_path[MAX_RCPT];
    int tx_rmt_path_len;
};

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

void abort_cleanup(int fd)
{
    write_all(fd, "451 Requested action aborted: local error in processing\r\n", 57);
    cleanup(fd);
}

size_t save_path(struct str_iterator* str_it, struct input_iterator* it, int n)
{
    char c;
    size_t s_it_r;

    /* Find the start */
    for (c = input_next(it); c == '<' || c == ' '; c = input_next(it)) { }

    for (s_it_r = str_it->str_offset; c != '\r' && c != '>'; c = input_next(it), str_it->str_offset++) {

        if (n < 0) abort_cleanup(it->fd);

        /* Forward slashes are illegal */
        if (c == '/')
            continue;

        /* Resize */
        if (str_it->str_offset >= str_it->str->str_size)
            if ((str_it->str = str_resize(str_it->str, str_it->str->str_size * 1.5)) == NULL)
                abort_cleanup(it->fd);
    
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

void mail_command(struct smtp_context* sc, struct input_iterator* it, struct smtp_response* res_r)
{
    /* Ordering */
    if (sc->smtp_tx.tx_rev_path != 0)
        return unknown_command(sc, it, res_r);

    /* Setup a transaction */
    sc->smtp_tx.tx_str_head.str = str_resize(NULL, DEFAULT_STR_SIZE);

    /* Save reverse path */
    sc->smtp_tx.tx_rev_path = save_path(&sc->smtp_tx.tx_str_head, it, MAX_PATH_LEN);

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

    /* Push fwd_path to rmt_path scatter/gather */
    //sc->smtp_tx.tx_it_head.p[-1] = ',';

    /* Need to forward */
    if (sc->smtp_tx.tx_rmt_path_len == 1) {
        /* First remote path setup */
        /*memcpy(spool_path, "spool/tmp/", 10);
        memcpy(spool_path + 10, sc->smtp_tx.tx_r, RAND_LEN);
        sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len++] = spool_path;*/

        /* Push rev_path to rmt_path scatter/gather */
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
            abort_cleanup(it->fd);

        ; // TODO SMTP headers
    }

    write_all(it->fd, "354 Start mail input; end with <CRLF>.<CRLF>\r\n", 46);

    for (const char* t = DATA_END_TOKEN;;) {
        char* p;

        /* Read some */
        if ((p = read_some(it->fd, it->input_buffer, sizeof(it->input_buffer))) == it->input_buffer)
            cleanup(it->fd);
        p = it->input_buffer;

        /* Find the DATA_END_TOKEN */
        for (; *p != '\0' && *t != '\0'; p++, t++) {

            if (*p == *t) continue;

            if (t != DATA_END_TOKEN) {
                /* Make up for unwritten DATA_END_TOKEN pieces */
                for (int i = 0; i < sc->smtp_tx.tx_fwd_path_len; i++)
                    write_all(ffds[i], DATA_END_TOKEN, t - DATA_END_TOKEN);
                p--;
            }

            /* Reset t */
            t = DATA_END_TOKEN - 1;
        }

        /* Write to all files */
        if ((t - DATA_END_TOKEN) < (p - it->input_buffer))
            for (int i = 0; i < sc->smtp_tx.tx_fwd_path_len; i++)
                write_all(ffds[i], it->input_buffer, p - it->input_buffer - (t - DATA_END_TOKEN));

        if (*t == '\0') break;
    }
    it->input_buffer[it->i] = '\0';

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
            abort_cleanup(it->fd); // TODO rollback half commits

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
    write_all(it->fd, "250 OK\r\n", 8);
    cleanup(it->fd);
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

    it.fd = fd;
    it.i = 0;
    it.input_buffer[0] = '\0';

    /* Greeting */
    write_all(fd, greeting, greeting_len);
    
    for (;;) {
        struct smtp_response sr;

        command_mapper(&it)(&sc, &it, &sr);
        write_all(it.fd, sr.res, sr.res_len);

        input_find(&it, '\n');
    }
}

int main(int argc, char **argv)
{
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
