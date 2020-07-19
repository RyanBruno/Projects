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

#define STRBCPY(d, s) \
        char* h = d; \
        char* p = s; \
        for (; *p != '\0'; h--, p--) \
            *h = *p; \

#define MAX_RCPT 100
#define MAX_DATA 16384
#define BACKLOG  10
#define RAND_LEN 64
/* Coming soon */
#define authorized 0

const char* DATA_END_TOKEN = "\r\n.\r\n";

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

/* Random */
int rfd;

char greeting[24] = "220 SMTP Service Ready\r\n";
int greeting_len = 24;

struct smtp_context {
    /* Buffer */
    int sfd;
    char path[197];
    char r[64];
    char path_buf[1024];
    char* path_buf_head;
    char* fwd_path[MAX_RCPT];
    int fwd_path_len;
};

struct input_iterator {
    int fd;
    int i;
    char input_buffer[1024];
};

char* read_some(int fd, char* b, size_t s)
{
    int r;

    for (s--;;) {

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

void write_all(int fd, const char* b, size_t s)
{
    int w;

    for (;;) {
        w = write(fd, b, s);

        if (w < 1) {
            if (errno == EAGAIN || errno == EINTR) continue;
            exit(-1);
        }

        b += w;
        s -= w;

        if (s == 0) return;
    }
}

char next(struct input_iterator* it)
{
    if (it->input_buffer[it->i] != '\0')
        return it->input_buffer[it->i++];

    if (read_some(it->fd, it->input_buffer, sizeof(it->input_buffer)) == it->input_buffer)
        exit(-1);

    it->i = 0;
    return it->input_buffer[it->i++];
}

void find(struct input_iterator* it, char c)
{
    for (char s = next(it); s != c; s = next(it)) { }
}

char* save_path(char* dest, struct input_iterator* it, int n)
{
    char c;
    char* d;
    int l;
    /* Find the start */
    for (c = next(it); c == '<' || c == ' '; c = next(it)) { }

    n--;

    for (d = dest + sizeof(int); c != '\r' && c != '>'; c = next(it), d++, n--) {

        if (n < 1) exit(-1);

        /* Forward slashes are illegal */
        if (c == '/') {
            d--;
            n++;
            continue;
        }
        *d = c;
    }

    l = d - dest - sizeof(int);
    memcpy(dest, &l, sizeof(int));

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
    /* Save reverse path */
    sc->path_buf_head = save_path(sc->path_buf, it, sizeof(sc->path_buf)) + 1;
    sc->fwd_path_len = 0;

    write_all(it->fd, "250 OK\r\n", 8);

    {/* Setup Random */
        for (char* e = sc->path + 133; e - sc->path < sizeof(sc->path) - 1;)
            e += read(rfd, e, sizeof(sc->path) - (e - sc->path));

        /* Encode and terminate Random */
        for (int j = sizeof(sc->path) - 64; j < sizeof(sc->path) - 1; j++)
            { sc->path[j] = (abs(sc->path[j]) % 26) + 97; }
        sc->path[sizeof(sc->path) - 1] = '\0';
    }

    return sc;
}

struct smtp_context* rcpt_command(struct smtp_context* sc, struct input_iterator* it)
{

    char *p;
    char *h;
    struct stat st;
    int l;

    /* Ordering */
    if (sc->path_buf_head == NULL)
        return unknown_command(sc, it);

    /* Save forward_path */
    h = sc->path_buf_head;
    sc->path_buf_head = save_path(h, it, sizeof(sc->path_buf) - (sc->path_buf_head - sc->path_buf));
    sc->path_buf_head[0] = '\0';

    if (stat(h + sizeof(int), &st) == 0) {
        /* Push forward rcpt */
        sc->fwd_path[sc->fwd_path_len++] = h;

        write_all(it->fd, "250 OK\r\n", 8);
        return sc;
    }

    if (!authorized) {
        /* Reset head */
        sc->path_buf_head = h;
        write_all(it->fd, "551 User not local\r\n", 20);
        return sc;
    }

    /* Need to forward */
    if (sc->sfd < 0) {
        /* First remote path setup */
        sc->fwd_path[sc->fwd_path_len++] = "\0spool" + 5;
        /* Build spool path */
        memcpy(sc->path + 118, "spool/tmp/", 10);

        /* Open spool file */
        if ((sc->sfd = open(sc->path + 118, O_CREAT | O_WRONLY, 0644)) < 0)
            exit(-1);

        memcpy(&l, sc->path_buf, sizeof(int));
        sc->path_buf[l + sizeof(int)] = ',';
        write_all(sc->sfd, sc->path_buf + sizeof(int), l);
    }

    /* Write remote path to spool file */
    sc->path_buf_head[0] = ',';
    write_all(sc->sfd, h + sizeof(int), (sc->path_buf_head - h) + 1);
    write_all(it->fd, "251 User not local; will forward\r\n", 34);
}

struct smtp_context* data_command(struct smtp_context* sc, struct input_iterator* it)
{
    int ffds[MAX_RCPT];

    /* Ordering */
    if (sc->fwd_path_len < 1)
        return unknown_command(sc, it);

    /*if (sc->sfd != -1)
        fwrite(sfd, "\r\n", 2);*/

    /* Prep Path */
    memcpy(sc->path + 128, "/tmp/", 5);

    /* Open tmp files */
    for (int i = 0; i < sc->fwd_path_len; i++) {
        int l;

        //fps[i] = sfd;

        //if (strncmp("\0spool", sc->fwd_path[i] - 5, 7)) {
            /* Write forward path backwards */
        memcpy(&l, sc->fwd_path[i], sizeof(int));
        memcpy(sc->path + 128 - l, sc->fwd_path[i] + sizeof(int), l);

            /* Open the temp file */
            if ((ffds[i] = open(sc->path + 128 - l, O_CREAT | O_EXCL | O_WRONLY, 0644)) < 0)
                exit(-1);
        //}

        // TODO SMTP headers
    }

    write_all(it->fd, "354 Start mail input; end with <CRLF>.<CRLF>\r\n", 46);

    {
        const char* t;
        size_t s = 0;

        t = DATA_END_TOKEN;
        s = 0;

        for (;;) {
            char* end;

            end = read_some(it->fd, it->input_buffer, sizeof(it->input_buffer));

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
        char nb[197];
        int l;

        memcpy(&l, sc->fwd_path[i], sizeof(int));
        memcpy(sc->path + 128 - l, sc->fwd_path[i] + sizeof(int), l);

        /* Copy tmp file path to nb */
        memcpy(nb, sc->path + 128 - l, l + 64 + 5);
        memcpy(nb + l + 1, "new", 3);

        /* Move file from tmp to new */
        if (rename(sc->path + 128 - l, nb) < 0)
            exit(-1); // TODO rollback half commits

        close(ffds[i]);
    }

    write_all(it->fd, "250 will deliver\r\n", 18);

    sc->path_buf_head = NULL;
    sc->fwd_path_len = 0;
    return sc;
}

struct smtp_context* rset_command(struct smtp_context* sc, struct input_iterator* it)
{
    sc->path_buf_head = NULL;
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
    exit(-1);
}

int stritmatch(char* str, struct input_iterator* it, int n)
{
    for (char c; n > 0; str++, n--) {
        c = next(it);

        if (toupper(c) != *str) return 0;
        if (*str == '\0') break;
    }
    return 1;
}

struct smtp_context*(*command_mapper(struct input_iterator* it))(struct smtp_context*, struct input_iterator*)
{
    switch (toupper(next(it))) {
        case 'H':
            if (stritmatch("ELO", it, 3))       return helo_command;
        case 'M':
            if (stritmatch("AIL FROM:", it, 9)) return mail_command;
        case 'R':
            switch (toupper(next(it))) {
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

    sc.path_buf_head = NULL;
    sc.fwd_path_len = 0;
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
