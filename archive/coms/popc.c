#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>

#define OK(fd, ...) { \
    read_some(fd, buf, sizeof(buf)); \
    for (p = buf; *p != '\n'; p++)  \
        if (*p == '\0') { if (*p == '\0') exit(0); \
            read_some(fd, p, sizeof(buf) - (p - buf)); p--; } \
    printf("%s\n", buf); \
    if (buf[0] != '+') { fprintf(stderr, __VA_ARGS__); exit(0); }}

#define SEND(fd, ...) {\
    int len; \
    len = snprintf(buf, sizeof(buf), __VA_ARGS__); \
    if (len > sizeof(buf)) exit(1); \
    printf("%s\n", buf); \
    write_all(fd, buf, len);}

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

const char* DATA_END_TOKEN = "\r\n.\r\n";

/* Random */
int rfd;

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

char* strmcpy(char* dest, const char* src, size_t s)
{
    for (; s > 0; dest++, src++, s--) {
        *dest = *src;

        if (*src == '\0')
            return dest;
    }
    exit(-2);
}

int main(int argc, char **argv)
{
    int fd;
    struct addrinfo *a;

    char* host = NULL;
    char* port = NULL;
    char* user = NULL;
    char* pass = NULL;

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
            user = *++argv;
            break;
        case 'k':
            pass = *++argv;
            break;
        default:
            goto help;
        }
    }

    if (host == NULL || port == NULL || user == NULL || pass == NULL)
        goto help;

    rfd = open("/dev/urandom", O_RDONLY);


    /* Socket Setup */
    if (getaddrinfo(host, port, &ai_hints, &a) != 0 ||
        (fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol)) == -1 ||
        connect(fd, a->ai_addr, a->ai_addrlen) == -1)
    {
        fprintf(stderr, "Could not connect\n");
        return -1;
    }

    freeaddrinfo(a);

    /* Move to popc() */
    char buf[512];
    char r[64];
    char* p;

    /* Wait for greeting */
    OK(fd, "No greeting");

    /* USER & PASS */
    SEND(fd, "USER %s\r\n", user);
    OK(fd, "Username not accepted.\n");
    SEND(fd, "PASS %s\r\n", pass);
    OK(fd, "Could not login.\n");

    SEND(fd, "STAT\r\n");
    OK(fd, "Could not stat.\n");

    /* RETR & DELE */
    for (long int i = strtol(buf + 3, NULL, 10); i > 0; i--) {
        int ffd;
        char* p;
        const char* t;
        char nb[128];

        /* RETR */
        SEND(fd, "RETR %ld\r\n", i);
        for (char* e = buf; e - buf < 5;)
            e = read_some(fd, e, 6 - (e - buf));
        printf("--%s\n", buf);
        if (buf[0] != '+') { fprintf(stderr, "Cannot Retr\n"); exit(-3); }

        /* Setup Random */
        for (char* e = r; e - r < sizeof(r) - 1;)
            e = read_some(rfd, e, sizeof(r) - (e - r));

        /* Encode and terminate Random */
        for (int j = 0; j < sizeof(r); j++) { r[j] = (abs(r[j]) % 26) + 97; }
        r[sizeof(r) - 1] = '\0';

        /* Build path to tmp file */
        p = strmcpy(buf, user, sizeof(buf));
        memcpy(p, "/tmp/", 5);
        p += 5;
        strmcpy(p, r, sizeof(buf) - (p - buf));

        /* Open the tmp file */
        if ((ffd = open(buf, O_CREAT | O_EXCL | O_WRONLY, 0640)) < 0)
            exit(-4);

        for (t = DATA_END_TOKEN ;;) {
            char* end;

            end = read_some(fd, buf, sizeof(buf));

            end -= 5;
            if (end < buf)
                end = buf;

            /* Find the DATA_END_TOKEN */
            for (; *end != '\0' && *t != '\0'; end++, t++) {

                if (*end == *t) continue;

                if (t != DATA_END_TOKEN) {
                    /* Make up for unwritten DATA_END_TOKEN pieces */
                    write_all(ffd, DATA_END_TOKEN, t - DATA_END_TOKEN);
                    end--;
                }

                /* Reset t */
                t = DATA_END_TOKEN - 1;
            }

            write_all(ffd, buf, end - buf - (t - DATA_END_TOKEN));

            if (*t == '\0') break;
        }

        /* Write to all files */

        close(ffd);

        /* Build path to tmp file */
        p = strmcpy(buf, user, sizeof(buf));
        memcpy(p, "/new/", 5);
        strmcpy(p + 5, r, sizeof(buf) - (p - buf));
        strmcpy(nb, buf, sizeof(nb));
        memcpy(p, "/tmp/", 5);

        /* Move from tmp to new */
        if (rename(buf, nb) < 0)
            continue;

        /* DELE */
        SEND(fd, "DELE %ld\r\n", i);
        OK(fd, "Cannot delete.\n");
    }

    SEND(fd, "QUIT\r\n");
    OK(fd, "Quit Failed.\nTransaction may not have been commited!\n");

    close(fd);
    close(rfd);

    return 0;

help:
    printf("popc -h <host> -p <port> -u <user> -k <key>\n");
    return -1;
}
