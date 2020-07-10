#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>

#define RUN_OFF(p) { \
    char* r = p; \
    for (;; r++) { \
        if (*r == '\0') { \
            read_some(fd, head, sizeof(buf) - (head - buf)); \
            r = head;  \
        }\
        if (*r == '\0')  \
            cleanup(fd); \
        if (*r == '\n') \
            break; \
        }} 

#define FIND_MAIL(b, p) \
    struct mail_t* m = md_head; \
    for (p = b;; p++) { \
        if (*p == '\0') read_some(fd, p, sizeof(buf) - (buf - p)); \
        if (*p == '\r' || *p == '\n' || *p == ' ') break;} \
    long int n = strtol(b, &p, 10); \
    for (; n > 1 && m != NULL; n--) { m = m->m_next; } \
    if (m == NULL) { write_all(fd, "-ERR\r\n", 6); continue; }

#define FOR_EACH(m) \
    for (m = md_head; m != NULL; m = m->m_next)

#define BACKLOG 10

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

const char* password =      "password";
size_t password_l    =      8;

struct mail_t {
    const char* m_fname;
    short m_del;
    int m_octets;
    struct mail_t* m_next;
};

void cleanup(int fd) { close(fd); exit(0); }

struct mail_t* md_head = NULL;

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

void write_all(int fd, char* b, size_t s)
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

char* strmcpy(char* dest, const char* src, size_t s, int fd)
{
    for (; s > 0; dest++, src++, s--) {
        *dest = *src;

        if (*src == '\0')
            return dest;
    }
    cleanup(fd);
}

void push_mail(char path[6], char* head, char* maildrop, int fd, char* buf, size_t s)
{
    DIR *d;
    struct dirent* de;

    memcpy(head - 1, path, 6);

    if ((d = opendir(maildrop)) == NULL)
        goto cleanup;

    while ((de = readdir(d)) != NULL) {
        struct stat st;
        struct mail_t* m;

        /* Build the path name */
        strmcpy(head + 4, de->d_name, s - (head - buf) - 4, fd);

        if (de->d_type != DT_REG ||
            stat(maildrop, &st) < 0)
            continue;

        /* Fill in mail info */
        m = malloc(sizeof(struct mail_t));
        m->m_fname = strdup(de->d_name);
        m->m_del = 0;
        m->m_octets = st.st_size;

        /* Push to linked list */
        m->m_next = md_head;
        md_head = m;

    }
    closedir(d);
cleanup:
    memcpy(head - 1, "\0", 2);
}

int match(const char* cmd, char* head)
{
    for (; *cmd != '\0'; cmd++, head++) {
        char c = *head;

        if (c > 96) c -= 32;
        if (c != *cmd) return 0;
    }
    return 1;
}

void pop3(int fd)
{

    /* Buffers */
    char buf[4096];
    char* head;

    /* Register(s) */
    char* maildrop;

    head = buf;
    maildrop = NULL;

    /* Greeting */
    write_all(fd, "+OK POP3 server ready\r\n", 23);

    /* AUTHORIZATION */
    do {
        *head = '\0';
        /* Prelaod the command */
        for (char* p = head; ; p++) {
            if (*p == '\0') read_some(fd, head, sizeof(buf) - (head - buf));
            if (*p == '\0') cleanup(fd);
            if (*p == ' ' || *p == '\n') break;
        }

        if (maildrop == NULL && match("USER ", head)) {
            char* p;

            /* Save the maildrop */
            maildrop = head;

            /* Skip to the start */
            for (p = head + 5; *p == '<' || *p == ' '; p++) { }

            for (;; head++, p++) {

                if (*p == '\0') {
                    read_some(fd, head, sizeof(buf) - (head - buf));
                    p = head;
                }

                if (*p == '\r' || *p == '\n' || *p == '>')
                    break;

                *head = *p;
            }
            *head = '\0';
            head++;

            RUN_OFF(p);
            /* For Security USER always returns +OK */
            write_all(fd, "+OK\r\n", 5);

        } else if (maildrop != NULL && match("PASS ", head)) {

            struct stat st;
            char* pass = head + 5;

            // TODO preload & encode? pass */
            /* Check if maildrop exists */
            if (stat(maildrop, &st) < 0 ||
            /* Check password */
                strncmp(password, pass, password_l) != 0)
            {
                RUN_OFF(pass);
                write_all(fd, "-ERR\r\n", 6);
                continue;
            }

            /* Build maildrop in memory */
            push_mail("/new/", head, maildrop, fd, buf, sizeof(buf));
            push_mail("/cur/", head, maildrop, fd, buf, sizeof(buf));

            write_all(fd, "+OK\r\n", 5);

            break;

        } else if (match("QUIT", head)) {

            write_all(fd, "+OK\r\n", 5);
            cleanup(fd);

        } else {
            RUN_OFF(head);
            write_all(fd, "-ERR\r\n", 6);
        }

    } while (1);

    /* TRANSACTION */
    do {
        *head = '\0';
        /* Prelaod the command */
        for (char* p = head; ; p++) {
            if (*p == '\0') read_some(fd, head, sizeof(buf) - (head - buf));
            if (*p == '\0') cleanup(fd);
            if (*p == ' ' || *p == '\n') break;
        }

        if (match("STAT", head)) {
            int num = 0;
            int octets = 0;
            int len;
            struct mail_t* m;

            FOR_EACH(m) {
                /* For all non-deleted messages... */
                if (m->m_del)
                    continue;

                num++;
                octets += m->m_octets;
            }

            RUN_OFF(head + 4);
            len = sprintf(head, "+OK %d %d\r\n", num, octets);
            write_all(fd, head, len);

        } else if (match("LIST ", head) || match("UIDL ", head)) {

            char* p;
            int len;
            int l = *head == 'L';

            FIND_MAIL(head + 5, p);

            RUN_OFF(head + 5);
            if (l)
                len = sprintf(head, "+OK %ld %d\r\n", n, m->m_octets);
            else
                len = sprintf(head, "+OK %ld %s\r\n", n, m->m_fname);

            write_all(fd, head, len);

        } else if (match("LIST", head) || match("UIDL", head)) {

            int len;
            struct mail_t* m;
            int n;
            int l = *head == 'L';

            write_all(fd, "+OK\r\n", 5);
            RUN_OFF(head + 4);

            /* List all mail in the maildrop with num and size */
            for (n = 1, m = md_head; m != NULL; n++, m = m->m_next) {
                /* For all non-deleted messages... */
                if (m->m_del)
                    continue;

                if (l)
                    len = sprintf(head,  "%d %d\r\n", n, m->m_octets);
                else
                    len = sprintf(head,  "%d %s\r\n", n, m->m_fname);
                write_all(fd, head, len);
            }

            write_all(fd, ".\r\n", 3);

        } else if (match("RETR", head) || match("TOP ", head)) {

            char* p;
            int ffd;
            struct stat st;

            FIND_MAIL(head + 4, p);

            RUN_OFF(p);

            /* Find the file; Check new/ then cur/ then del/ */
            memcpy(head - 1, "/new/", 5);
            strmcpy(head + 4, m->m_fname, sizeof(buf) - (head + 4 - buf), fd);

            /* First check new/ */
            if (stat(maildrop, &st) < 0) { memcpy(head - 1, "/cur/", 5);
            /* Then cur/ */
            if (stat(maildrop, &st) < 0) { memcpy(head - 1, "/del/", 5); 
            /* Then del/ */
            if (stat(maildrop, &st) < 0) { cleanup(fd); }}}

            /* Open the file */
            if ((ffd = open(buf ,O_RDONLY)) == -1)
                cleanup(fd);

            write_all(fd, "+OK\r\n", 5);

            /* Send entire file to client */
            while (1) {
                char* end;

                end = read_some(ffd, head, sizeof(buf) - (head - buf));
                if (end == head) break;
                write_all(fd, head, end - head);
            }

            close(ffd);
            memcpy(head - 1, "\0", 2);

            write_all(fd, "\r\n.\r\n", 5);

        } else if (match("DELE ", head)) {
            char* p;

            FIND_MAIL(head + 5, p);

            /* Mark deleted */
            m->m_del = 1;

            RUN_OFF(head + 5);
            write_all(fd, "+OK\r\n", 5);

        } else if (match("NOOP", head)) {

            RUN_OFF(head + 4);
            write_all(fd, "+OK\r\n", 5);

        } else if (match("RSET", head)) {
            struct mail_t* m;

            /* Mark all mail undeleted */
            FOR_EACH(m) { m->m_del = 0; }

            RUN_OFF(head + 4);
            write_all(fd, "+OK\r\n", 5);

        } else if (match("QUIT", head)) {
            struct mail_t* m;

            FOR_EACH(m) {
                char df[128];

                /* For all deleted messages... */
                if (!m->m_del)
                    continue;

                /* Copy filename */
                strmcpy(head + 4, m->m_fname, sizeof(buf) - (head - buf) - 3, fd);
                memcpy(head - 1, "/new/", 5);

                /* Delete buffer */
                strmcpy(df, buf, sizeof(df), fd);
                memcpy(df + (head - buf) - 1, "/del/", 5);

                /* First look in new/ */
                rename(buf, df);

                /* Then cur/ */
                memcpy(head, "cur", 3);
                rename(buf, df);
            }

            write_all(fd, "+OK\r\n", 5);
            cleanup(fd);

        } else {
            RUN_OFF(head);
            write_all(fd, "-ERR\r\n", 6);
        }

    } while (1);
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

    do {
        int fd;

        errno = 0;
        if ((fd = accept(afd, NULL, NULL)) == -1)
            break;

        if (fork() == 0) {
            pop3(fd);
            close(fd);
            exit(0);
        }

    } while (errno == 0 || errno == EINTR);

    return -1;

help:
    printf("popd -h <host> -p <port>\n");
    return -1;
}
