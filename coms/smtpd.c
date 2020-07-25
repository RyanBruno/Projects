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

#include <arpa/inet.h>
#include <netdb.h>

#define BACKLOG  10
#define MAX_RCPT 100
#define MAX_PATH_LEN 256
#define MAX_PATH_BUF MAX_PATH_LEN + RAND_LEN + 5
#define RAND_LEN 64
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

void linked_blocks_char_put(struct linked_blocks_char_iterator* it, char c)
{
    if (it->p - it->block->data >= sizeof(it->block->data)) {
        it->block->next = malloc(sizeof(struct linked_blocks));
        it->block = it->block->next;
        it->block->next = NULL;
        it->p = it->block->data;
    }

    *it->p = c;
    it->p++;
}

void linked_blocks_free(struct linked_blocks* lb)
{
    struct linked_blocks* tmp;

    for (tmp = lb; tmp != NULL; free(lb)) { tmp = lb->next; }
}


struct smtp_context {
    /* Buffer */
    int sfd;
    char r[RAND_LEN];
    struct linked_blocks* blocks;
    struct linked_blocks_char_iterator it_head;
    struct linked_blocks_char_iterator rev_path;
    struct linked_blocks_char_iterator fwd_path[MAX_RCPT];
    int fwd_path_len;
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

struct linked_blocks_char_iterator
save_path(struct linked_blocks_char_iterator* dest, struct input_iterator* it, int n)
{
    char c;
    struct linked_blocks_char_iterator d;
    int i;

    d = *dest;
    linked_blocks_char_put(dest, '\0');

    /* Find the start */
    for (c = input_next(it); c == '<' || c == ' '; c = input_next(it)) { }

    for (i = 0; c != '\r' && c != '>'; c = input_next(it)) {

        if (i >= n) abort_cleanup(it->fd);

        /* Forward slashes are illegal */
        if (c == '/')
            continue;

        linked_blocks_char_put(dest, c);
        i++;
    }

    linked_blocks_char_put(dest, '\0');
    d.p[0] = i;

    return d;
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
    if (sc->it_head.p != sc->it_head.block->data)
        return unknown_command(sc, it);

    /* Save reverse path */
    sc->rev_path = save_path(&sc->it_head, it, MAX_PATH_LEN);
    sc->fwd_path_len = 0;

    write_all(it->fd, "250 OK\r\n", 8);

    {/* Setup Random */
        for (char* e = sc->r; e - sc->r < sizeof(sc->r) - 1;)
            e += read(rfd, e, sizeof(sc->r) - (e - sc->r));

        /* Encode and terminate Random */
        for (int j = 0; j < sizeof(sc->r) - 1; j++)
            { sc->r[j] = (abs(sc->r[j]) % 26) + 97; }
        sc->r[sizeof(sc->r) - 1] = '\0';
    }

    return sc;
}

struct smtp_context* rcpt_command(struct smtp_context* sc, struct input_iterator* it)
{

    char *p;
    struct stat st;
    char b[MAX_PATH_LEN];

    /* Ordering */
    if (sc->it_head.p == sc->it_head.block->data)
        return unknown_command(sc, it);

    if (sc->fwd_path_len >= MAX_RCPT)
        exit(-1);

    /* Save forward_path */
    sc->fwd_path[sc->fwd_path_len] = save_path(&sc->it_head, it, MAX_PATH_LEN);
    linked_blocks_str_print(b, sc->fwd_path[sc->fwd_path_len], sc->fwd_path[sc->fwd_path_len].p[0] + 2);

    if (stat(b + 1, &st) == 0) {

        /* Push forward rcpt */
        sc->fwd_path_len++;

        write_all(it->fd, "250 OK\r\n", 8);
        return sc;
    }

    if (!authorized) {
        /* TODO Reset head */
        write_all(it->fd, "551 User not local\r\n", 20);
        return sc;
    }

    /* Need to forward */
    if (sc->sfd < 0) {
        /* First remote path setup */
        /*memcpy(sc->fwd_path[sc->fwd_path_len], "spool/tmp/", 10);
        memcpy(sc->fwd_path[sc->fwd_path_len] + 10, sc->r, RAND_LEN);
        sc->fwd_path_len++;*/

        /* Open spool file */
        /*if ((sc->sfd = open(sc->fwd_path[sc->fwd_path_len], O_CREAT | O_WRONLY, 0644)) < 0)
            abort_cleanup(it->fd);*/

        /*memcpy(&l, sc->path_buf, sizeof(int));
        sc->path_buf[l + sizeof(int)] = ',';
        write_all(sc->sfd, sc->path_buf + sizeof(int), l);*/
    }

    /* Write remote path to spool file */
    /*sc->path_buf_head[0] = ',';
    write_all(sc->sfd, h + sizeof(int), (sc->path_buf_head - h) + 1);*/
    write_all(it->fd, "251 User not local; will forward\r\n", 34);
    return sc;
}

struct smtp_context* data_command(struct smtp_context* sc, struct input_iterator* it)
{
    int ffds[MAX_RCPT];
    char b[MAX_PATH_BUF];

    /* Ordering */
    if (sc->fwd_path_len < 1)
        return unknown_command(sc, it);

    if (sc->sfd != -1)
        write_all(sc->sfd, "\r\n", 2);

    memcpy(b + MAX_PATH_LEN + 5, sc->r, sizeof(sc->r));
    memcpy(b + MAX_PATH_LEN, "/tmp/", 5);

    /* Open tmp files */
    for (int i = 0; i < sc->fwd_path_len; i++) {
        char* s = b + MAX_PATH_LEN - sc->fwd_path[i].p[0] - 1;

        ffds[i] = sc->sfd;

        linked_blocks_str_print(s, sc->fwd_path[i], sc->fwd_path[i].p[0] + 1);

        if (!strncmp(s, "spool/", 6))
            goto HEADERS;

        /* Open the temp file */
        if ((ffds[i] = open(s + 1, O_CREAT /*| O_EXCL*/ | O_WRONLY, 0644)) < 0)
            abort_cleanup(it->fd);
HEADERS:
        ; // TODO SMTP headers
    }

    write_all(it->fd, "354 Start mail input; end with <CRLF>.<CRLF>\r\n", 46);

    {
        const char* t;

        t = DATA_END_TOKEN;

        for (;;) {
            char* end;

            if ((end = read_some(it->fd, it->input_buffer, sizeof(it->input_buffer))) == it->input_buffer)
                cleanup(it->fd);

            end -= 5;
            if (end < it->input_buffer)
                end = it->input_buffer;

            /* Find the DATA_END_TOKEN */
            for (; *end != '\0' && *t != '\0'; end++, t++) {

                if (*end == *t) continue;

                if (t != DATA_END_TOKEN) {
                    /* Make up for unwritten DATA_END_TOKEN pieces */
                    for (int i = 0; i < sc->fwd_path_len; i++)
                        write_all(ffds[i], DATA_END_TOKEN, t - DATA_END_TOKEN);
                    end--;
                }

                /* Reset t */
                t = DATA_END_TOKEN - 1;
            }

            /* Write to all files */
            if ((t - DATA_END_TOKEN) < (end - it->input_buffer))
                for (int i = 0; i < sc->fwd_path_len; i++)
                    write_all(ffds[i], it->input_buffer, end - it->input_buffer - (t - DATA_END_TOKEN));

            if (*t == '\0') break;
        }
    }
    it->input_buffer[it->i] = '\0';

    /* Move all files from tmp to new */
    for (int i = 0; i < sc->fwd_path_len; i++) {
        int o = MAX_PATH_LEN - sc->fwd_path[i].p[0] - 1;
        char nb[MAX_PATH_LEN - 1];

        linked_blocks_str_print(b + o++, sc->fwd_path[i], sc->fwd_path[i].p[0] + 1);
        /* Copy tmp file path to nb */
        memcpy(nb, b + o, sizeof(b) - o);
        memcpy(nb + sc->fwd_path[i].p[0] + 1, "new", 3);


        /* Move file from tmp to new */
        if (rename(b + o, nb) < 0)
            abort_cleanup(it->fd); // TODO rollback half commits

        /* Cleanup */
        close(ffds[i]);
    }

    write_all(it->fd, "250 will deliver\r\n", 18);

    if (sc->sfd != -1) close(sc->sfd);
    sc->sfd = -1;

    sc->it_head.block = sc->blocks;
    sc->it_head.p = sc->blocks->data;
    sc->fwd_path_len = 0;
    return sc;
}

struct smtp_context* rset_command(struct smtp_context* sc, struct input_iterator* it)
{
    if (sc->sfd != -1) close(sc->sfd);
    sc->sfd = -1;

    sc->it_head.block = sc->blocks;
    sc->it_head.p = sc->blocks->data;
    sc->fwd_path_len = 0;
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

    sc.sfd = -1;
    sc.fwd_path_len = 0;
    sc.it_head.block = malloc(sizeof(struct linked_blocks_char_iterator));
    sc.it_head.block->next = NULL;
    sc.it_head.p = sc.it_head.block->data;
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
        if ((fd = accept(afd, (struct sockaddr*) &pa, &pa_size)) == -1)
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
