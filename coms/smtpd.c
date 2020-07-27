#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#define BACKLOG  10
#define MAX_RCPT 100
#define MAX_PATH_LEN 256
#define RAND_LEN 64
#define PATH_EXTRA RAND_LEN + 5
/* Coming soon */
#define authorized 0

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

/* Random */
int rfd;

char greeting[24] = "220 SMTP Service Ready\r\n";
int greeting_len = 24;

char spool_path[10 + RAND_LEN];
const char* DATA_END_TOKEN = "\r\n.\r\n";

struct input_iterator {
    int fd;
    int i;
    char input_buffer[1000];
};

#define LINKED_BLOCK_SIZE 1024
struct linked_blocks {
    struct linked_blocks* next;
    char data[LINKED_BLOCK_SIZE];
};

struct linked_blocks_char_iterator {
    struct linked_blocks* block;
    char* p;
};

char linked_blocks_char_next(struct linked_blocks_char_iterator* it)
{
    char c;

    if (it->p - it->block->data >= sizeof(it->block->data)) {
        it->block = it->block->next;
        it->p = it->block->data;
    }
    c = it->p[0];
    it->p++;
    return c;
}

void linked_blocks_str_print(char* dest, struct linked_blocks_char_iterator it, int n)
{
    for (; n > 0; n--, dest++) { *dest = linked_blocks_char_next(&it); }
}

struct linked_blocks*
linked_blocks_init(void*(*alloc)(size_t s))
{
    struct linked_blocks* lb;

    lb = alloc(sizeof(struct linked_blocks));
    lb->next = NULL;
    return lb;
}

void linked_blocks_char_put(struct linked_blocks_char_iterator* it, char c, void*(*alloc)(size_t s))
{
    if (it->p - it->block->data >= sizeof(it->block->data)) {
        if (it->block->next == NULL)
            it->block->next = linked_blocks_init(alloc);
        it->block = it->block->next;
        it->p = it->block->data;
    }

    *it->p = c;
    it->p++;
}

void linked_blocks_free(struct linked_blocks* lb)
{
    struct linked_blocks* tmp;

    for (tmp = lb; tmp != NULL; free(lb)) { lb = tmp; tmp = lb->next; }
}

struct smtp_transaction {
    char tx_r[RAND_LEN];
    struct linked_blocks* tx_blocks;
    struct linked_blocks_char_iterator tx_it_head;
    char* tx_rev_path;
    char* tx_fwd_path[MAX_RCPT];
    int tx_fwd_path_len;
    struct iovec tx_rmt_path[MAX_RCPT];
    int tx_rmt_path_len;
};

struct smtp_context {
    struct smtp_transaction smtp_tx;
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

void cleanup(int fd) { close(fd); exit(0); }

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

void abort_cleanup(int fd)
{
    write_all(fd, "451 Requested action aborted: local error in processing\r\n", 57);
    cleanup(fd);
}

char input_next(struct input_iterator* it)
{
    if (it->input_buffer[it->i] != '\0')
        return it->input_buffer[it->i++];

    if (read_some(it->fd, it->input_buffer, sizeof(it->input_buffer)) == it->input_buffer)
        cleanup(it->fd);

    it->i = 0;
    return it->input_buffer[it->i++];
}

void find(struct input_iterator* it, char c)
{
    for (char s = input_next(it); s != c; s = input_next(it)) { }
}

char* save_path(struct linked_blocks_char_iterator* dest, struct input_iterator* it, int n)
{
    char c;
    char* s;

    /* Find the start */
    for (c = input_next(it); c == '<' || c == ' '; c = input_next(it)) { }

    for (s = dest->p; c != '\r' && c != '>'; c = input_next(it)) {

        if (n < 0) abort_cleanup(it->fd);

        /* Forward slashes are illegal */
        if (c == '/')
            continue;

        if (dest->p - dest->block->data >= sizeof(dest->block->data) - PATH_EXTRA) {
            int l;

            l = dest->p - s;
            dest->p += PATH_EXTRA;
            linked_blocks_char_put(dest, '\0', malloc);

            dest->p += l;
            memcpy(dest->block->data, s, l);
            s = dest->block->data;
        }
    
        linked_blocks_char_put(dest, c, malloc);
    }

    linked_blocks_char_put(dest, '\0', malloc);

    return s;
}

struct smtp_context* unknown_command(struct smtp_context* sc, struct input_iterator* it)
{
    write_all(it->fd, "500 Syntax error\r\n", 18);
    return sc;
}

struct smtp_context* helo_command(struct smtp_context* sc, struct input_iterator* it)
{
    write_all(it->fd, "250 SMTPd greats you\r\n", 22);
    return sc;
}

struct smtp_context* mail_command(struct smtp_context* sc, struct input_iterator* it)
{
    /* Ordering */
    if (sc->smtp_tx.tx_rev_path != NULL)
        return unknown_command(sc, it);

    /* Create a transaction */
    sc->smtp_tx.tx_blocks = linked_blocks_init(malloc);
    sc->smtp_tx.tx_it_head.block = sc->smtp_tx.tx_blocks;
    sc->smtp_tx.tx_it_head.p = sc->smtp_tx.tx_blocks->data;

    /* Save reverse path */
    sc->smtp_tx.tx_rev_path = save_path(&sc->smtp_tx.tx_it_head, it, MAX_PATH_LEN);

    write_all(it->fd, "250 OK\r\n", 8);

    /* Setup Random */
    for (char* e = sc->smtp_tx.tx_r; e - sc->smtp_tx.tx_r < sizeof(sc->smtp_tx.tx_r) - 1;)
        e += read(rfd, e, sizeof(sc->smtp_tx.tx_r) - (e - sc->smtp_tx.tx_r));

    /* Encode Random */
    for (int i = 0; i < sizeof(sc->smtp_tx.tx_r) - 1; i++)
        sc->smtp_tx.tx_r[i] = (abs(sc->smtp_tx.tx_r[i]) % 26) + 97;
    sc->smtp_tx.tx_r[sizeof(sc->smtp_tx.tx_r) - 1] = '\0';

    return sc;
}

struct smtp_context* rcpt_command(struct smtp_context* sc, struct input_iterator* it)
{
    struct linked_blocks_char_iterator p;
    struct stat st;

    /* Ordering */
    if (sc->smtp_tx.tx_rev_path == NULL)
        return unknown_command(sc, it);

    /* Too many rcpts */
    if (sc->smtp_tx.tx_fwd_path_len >= MAX_RCPT) // TODO Error message
        exit(-1);

    /* Save forward_path */
    p = sc->smtp_tx.tx_it_head;
    sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len] = save_path(&sc->smtp_tx.tx_it_head, it, MAX_PATH_LEN);

    if (stat(sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len], &st) == 0) {

        /* Print rest of /tmp/ path */
        memcpy(sc->smtp_tx.tx_it_head.p - 1, "/tmp/", 5);
        memcpy(sc->smtp_tx.tx_it_head.p + 4, sc->smtp_tx.tx_r, sizeof(sc->smtp_tx.tx_r));
        sc->smtp_tx.tx_it_head.p += sizeof(sc->smtp_tx.tx_r) + 5;

        /* Push forward rcpt */
        sc->smtp_tx.tx_fwd_path_len++;

        write_all(it->fd, "250 OK\r\n", 8);
        return sc;
    }

    if (!authorized) {
        /* Reset head */
        sc->smtp_tx.tx_it_head = p;

        write_all(it->fd, "551 User not local\r\n", 20);
        return sc;
    }

    /* Push fwd_path to rmt_path scatter/gather */
    sc->smtp_tx.tx_it_head.p[-1] = ',';

    /* Need to forward */
    if (sc->smtp_tx.tx_rmt_path_len == 1) {
        /* First remote path setup */
        memcpy(spool_path, "spool/tmp/", 10);
        memcpy(spool_path + 10, sc->smtp_tx.tx_r, RAND_LEN);
        sc->smtp_tx.tx_fwd_path[sc->smtp_tx.tx_fwd_path_len++] = spool_path;

        /* Push rev_path to rmt_path scatter/gather */
    }

    write_all(it->fd, "251 User not local; will forward\r\n", 34);
    return sc;
}

struct smtp_context* data_command(struct smtp_context* sc, struct input_iterator* it)
{
    int ffds[MAX_RCPT];

    /* Ordering */
    if (sc->smtp_tx.tx_fwd_path_len < 1)
        return unknown_command(sc, it);

    /* Open tmp files */
    for (int i = 0; i < sc->smtp_tx.tx_fwd_path_len; i++) {

        /* Open the temp file */
        if ((ffds[i] = open(sc->smtp_tx.tx_fwd_path[i], O_CREAT | O_EXCL | O_WRONLY, 0644)) < 0)
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
            if (sc->smtp_tx.tx_fwd_path[i][j] == '\0') { nb[j] = '\0'; break; }
            if (sc->smtp_tx.tx_fwd_path[i][j] == '/') { memcpy(nb + j, "/new/", 5); j += 4; continue; }
            nb[j] = sc->smtp_tx.tx_fwd_path[i][j];
        }

        /* Move file from tmp to new */
        if (rename(sc->smtp_tx.tx_fwd_path[i], nb) < 0)
            abort_cleanup(it->fd); // TODO rollback half commits

        /* Cleanup */
        close(ffds[i]);
    }

    write_all(it->fd, "250 will deliver\r\n", 18);

    linked_blocks_free(sc->smtp_tx.tx_blocks);
    memset(&sc->smtp_tx, '\0', sizeof(sc->smtp_tx));
    return sc;
}

struct smtp_context* rset_command(struct smtp_context* sc, struct input_iterator* it)
{
    linked_blocks_free(sc->smtp_tx.tx_blocks);
    memset(&sc->smtp_tx, '\0', sizeof(sc->smtp_tx));

    write_all(it->fd, "250 OK\r\n", 8);
    return sc;
}
struct smtp_context* noop_command(struct smtp_context* sc, struct input_iterator* it)
{
    write_all(it->fd, "250 OK\r\n", 8);
    return sc;
}
struct smtp_context* quit_command(struct smtp_context* sc, struct input_iterator* it)
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

struct smtp_context*(*command_mapper(struct input_iterator* it))(struct smtp_context*, struct input_iterator*)
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
        command_mapper(&it)(&sc, &it);
        find(&it, '\n');
    }
}

int main(int argc, char **argv)
{
    int afd;
    struct addrinfo *a;

    char* host = NULL;
    char* port = NULL;
    char* uid = NULL;
    char* gid = NULL;

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
        case 'u':
            uid = *++argv;
            break;
        case 'g':
            gid = *++argv;
            break;
        case 'o':
            greeting_len = snprintf(greeting, greeting_len, "220 %s\r\n", *++argv);
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

    if (gid != NULL) setgid(strtol(gid, NULL, 10));
    if (uid != NULL) setuid(strtol(uid, NULL, 10));

    signal(SIGCHLD, SIG_IGN);

    rfd = open("/dev/urandom", O_RDONLY);

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
    printf("smtpd -h <host> -p <port> -u <uid> -g <gid> -o <hostname>\n");
    return -1;
}
