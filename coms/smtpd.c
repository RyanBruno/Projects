#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netdb.h>

#define RUN_OFF(p) \
    char* z = p; \
    for (;; z++) { \
        if (*z == '\0') { \
            READ(head, END_GUARD(buf) - head); \
            z = head;  \
        }\
        if (*z == '\0')  \
            CLEANUP(); \
        if (*z == '\n') \
            break; } 

#define STRBCPY(d, s) \
        char* h = d; \
        char* p = s; \
        for (; *p != '\0'; h--, p--) \
            *h = *p; \

#define MAX_RCPT 100
#define MAX_DATA 16384
#define BACKLOG  10
#define RAND_LEN 64
#define END_GUARD(b) b + sizeof(b)
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

#define CLEANUP() { cleanup(fd); }
void cleanup(int fd) { close(fd); exit(0); }
char greeting[24] = "220 SMTP Service Ready\r\n";
int greeting_len = 24;

#define READ(b, s) read_some(fd, b, s)
#define READ_F(f, b, s) read_some(f, b, s)
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

#define WRITE(b, s) write_all(fd, b, s)
#define WRITE_F(f, b, s) write_all(f, b, s)
void write_all(int fd, const char* b, size_t s)
{
    for (int w;;) {

        w = write(fd, b, s);

        if (w < 1) {
            if (errno == EAGAIN || errno == EINTR) continue;
            CLEANUP();
        }

        b += w;
        s -= w;

        if (s == 0) return;
    }
}

#define SAVE(p) save_path(p, &head, fd, buf, sizeof(buf))
char* save_path(char* ptr, char** head, int fd, char* buf, size_t s)
{
    /* Find the start */
    for (; *ptr == '<' || *ptr == ' '; ptr++) { }

    for (;; (*head)++, ptr++) {

        if (*ptr == '\0') {
            READ(*head, buf + s - RAND_LEN - *head);
            ptr = *head;
        }

        if (*ptr == '/' || *ptr == '\0') CLEANUP();

        if (*ptr == '\r' || *ptr == '\n' || *ptr == '>')
            break;

        **head = *ptr;
    }
    **head = '\0';
    (*head)++;

    return ptr;
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

int smtp(int fd, struct sockaddr_in* pa)
{
    /* Registers */
    char* host;
    char* rev_path;
    char* fwd_path[MAX_RCPT];
    unsigned short fwd_path_len;

    /* Operational vars */
    int ffds[MAX_RCPT];
    int sfd = -1;

    /* Buffers */
    char* head;
    char path[197];
    char buf[12800];

    /* Reverse loopup addr */
    if (getnameinfo((struct sockaddr*) pa, sizeof(struct sockaddr_in),
                buf, 64, NULL, 0, 0))
        return 0;

    /* Greeting */
    WRITE(greeting, greeting_len);

    host = buf;
RSET:
    head = buf + strlen(host) + 1;
    rev_path = NULL;
    fwd_path_len = 0;
    if (sfd != -1) close(sfd);
    sfd = -1;

    do {
        *head = '\0';

        /* Prelaod the command */
        for (char* p = head; *p != ':' && *p != '\n'; p++) {
            if (*p == '\0') READ(p, END_GUARD(buf) - head);
            if (*p == '\0') CLEANUP();
        }

        if (match("HELO", head)) {

            RUN_OFF(head + 4);
            WRITE("250 SMTPd greats you\r\n", 22);

        } else if (rev_path == NULL && match("MAIL FROM:", head)) {

            char* p;

            /* Save reverse path, prepend its length */
            head++;
            rev_path = head;
            p = SAVE(head + 10);
            rev_path[-1] = (unsigned char) (head - rev_path - 1);

            RUN_OFF(p);
            WRITE("250 OK\r\n", 8);

            /* Setup Random */
            for (char* e = path + 133; e - path < sizeof(path) - 1;)
                e = READ_F(rfd, e, sizeof(path) - (e - path));

            /* Encode and terminate Random */
            for (int j = sizeof(path) - RAND_LEN; j < sizeof(path) - 1; j++)
                { path[j] = (abs(path[j]) % 26) + 97; }
            path[sizeof(path) - 1] = '\0';

        } else if (rev_path != NULL && match("RCPT TO:", head)) {

            char *p;
            char *h;
            struct stat st;

            /* Save forward_path */
            h = head;
            p = SAVE(head + 8);

            RUN_OFF(p);

            if (stat(h, &st) == 0) {
                /* Push forward rcpt */
                fwd_path[fwd_path_len++] = head - 2;

                WRITE("250 OK\r\n", 8);
                continue;
            }

            if (!authorized) {
                /* Will not forward, reset head */
                head = h;
                WRITE("551 User not local\r\n", 20);
                continue;
            }

            /* Need to forward */
            if (sfd < 0) {
                /* First remote path setup */
                fwd_path[fwd_path_len++] = "\0spool" + 5;
                /* Build spool path */
                memcpy(path + 118, "spool/tmp/", 10);

                /* Open spool file */
                if ((sfd = open(path + 118, O_CREAT | O_WRONLY, 0644)) < 0)
                    CLEANUP();

                WRITE_F(sfd, rev_path, (unsigned int) rev_path[-1]);
                WRITE_F(sfd, ",", 1);
            }

            /* Build a comma separated remote path string for smtpf */
            head[-1] = ',';
            WRITE_F(sfd, h, head - h);
            head[-1] = '\0';

            WRITE("251 User not local; will forward\r\n", 34);

        } else if (fwd_path_len > 0 && match("DATA", head)) {
            
            if (sfd != -1)
                WRITE_F(sfd, "\r\n", 2);

            RUN_OFF(head + 4);
            WRITE("354 Start mail input; end with <CRLF>.<CRLF>\r\n", 46);
            break;

        } else if (match("RSET", head)) {

            RUN_OFF(head + 4);
            WRITE("250 OK\r\n", 8);
            goto RSET;

        } else if (match("NOOP", head)) {

            RUN_OFF(head + 4);
            WRITE("250 OK\r\n", 8);

        } else if (match("QUIT", head)) {

            WRITE("250 OK\r\n", 8);
            CLEANUP();

        } else {

            RUN_OFF(head);
            WRITE("500 Syntax error\r\n", 18);
        }

    } while (1);

    /* Fill the end of the info buf with the end of the path */
    memcpy(path + 128, "/tmp/", 5);

    /* Open tmp files */
    for (int i = 0; i < fwd_path_len; i++) {

        ffds[i] = sfd;

        if (strncmp("\0spool", fwd_path[i] - 5, 7)) {

            /* Write forward path backwards */
            STRBCPY(path + 127, fwd_path[i]);

            /* Open the temp file */
            if ((ffds[i] = open(++h, O_CREAT | O_WRONLY, 0644)) < 0)
                CLEANUP();
        }

        // TODO SMTP headers
    }

    {
        const char* t;
        size_t s = 0;

        t = DATA_END_TOKEN;
        s = 0;

        for (;;) {
            char* end;

            end = READ(head, END_GUARD(buf) - head);

            end -= 5;
            if (end < head)
                end = head;

            /* Find the DATA_END_TOKEN */
            for (; *end != '\0' && *t != '\0'; end++, t++) {

                if (*end == *t) continue;

                if (t != DATA_END_TOKEN) {
                    /* Make up for unwritten DATA_END_TOKEN pieces */
                    for (int i = 0; i < fwd_path_len; i++)
                        WRITE_F(ffds[i], DATA_END_TOKEN, t - DATA_END_TOKEN);
                    end--;
                }

                /* Reset t */
                t = DATA_END_TOKEN - 1;
            }

            /* Write to all files */
            if ((t - DATA_END_TOKEN) < (end - head))
                for (int i = 0; i < fwd_path_len; i++)
                    WRITE_F(ffds[i], head, end - head - (t - DATA_END_TOKEN));

            if (*t == '\0') break;
        }
    }

    /* Move all files from tmp to new */
    for (int i = 0; i < fwd_path_len; i++) {
        char nb[197];

        /* Write forward path backwards */
        STRBCPY(path + 127, fwd_path[i]);

        /* Copy tmp file path to nb */
        h++;
        memcpy(nb, h, sizeof(path) - (h - path));
        memcpy(nb + (129 - (h - path)), "new", 3);

        /* Move file from tmp to new */
        if (rename(h, nb) < 0) CLEANUP(); // TODO rollback half commits
        close(ffds[i]);
    }

    WRITE("250 will deliver\r\n", 18);

    /* Fork and exec to forward remote mail */
    //if (rmt_path[0] != '\0' && fork() == 0) ;
        //execl("./smtpf", "./smtpf", r, NULL);

    goto RSET;

    return 0;
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
