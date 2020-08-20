#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/types.h>

#define MAX_RCPTS 30

#define FORMAT(...) \

#define SEND(...) { \
    int len; \
    if ((len = snprintf(data_buf, sizeof(data_buf), __VA_ARGS__)) >= sizeof(data_buf)) \
        exit(-1); \
    write_all(fd, data_buf, len); }

#define RUN_OFF(p) { \
    char* z = p; \
    for (;; z++) { \
        if (*z == '\0') { \
            read_some(fd, data_buf, sizeof(data_buf)); \
            z = data_buf;  \
        }\
        if (*z == '\0')  \
            return 0; \
        if (*z == '\n') \
            break; }}

char* greet = "HELO flappy.rbruno.com\r\n";
int greet_len = 24;
char* port = "25";

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

long int read_reply_code(int fd, char* b, size_t s)
{
    read_some(fd, b, s);
    for (char* p = b; *p != ' ' && *p != '\n'; p++) {
        if (*p == '\0')
            read_some(fd, p, s - (p - b));
        if (*p == '\0')
            exit(0);
    }
    return strtol(b, NULL, 10);
}

int resolve_first_mx(char* dname, char* mx_r, size_t s)
{
    unsigned char a[1024];
    int len;
    struct __res_state rs;
    ns_msg h;
    ns_rr rr;

    memset(&rs, '\0', sizeof(rs));
    if (res_ninit(&rs) < 0 ||
        /* This blocks */
        (len = res_nsearch(&rs, dname, C_IN, T_MX, a, sizeof(a))) < 0 ||
        ns_initparse(a, len, &h) < 0 ||
        (len = ns_msg_count(h, ns_s_an)) < 0)
    { return -1; }

    for (int i = 0; i < len; i++) {
        if (ns_parserr(&h, ns_s_an, i, &rr))
            continue;

        dn_expand(ns_msg_base(h),
                  ns_msg_end(h),
                  ns_rr_rdata(rr) + NS_INT16SZ,
                  mx_r,
                  s);
        return 0;
    }

    return -1;
}

int main(int argc, char *argv[])
{
    int ffd;
    int fd;
    char target_mx_host[128];
    char buf[2048];
    char data_buf[2048];
    char* data;
    char* data_end;
    off_t f_ptr;

    char* rcpt_set[MAX_RCPTS];
    int rcpt_set_i = 0;
    char* bounce_set[MAX_RCPTS];
    int bounce_set_i = 0;
    char* hold_set[MAX_RCPTS];
    int hold_set_i = 0;

    /* Open file */
    if ((ffd = open(argv[1], O_RDONLY)) == -1) {
        printf("File not found\n");
        return -1;
    }

    buf[0] = '\0';
    /* Read until '\n' */
    char* h = NULL;
    rcpt_set[rcpt_set_i++] = buf;
    for (char* e = buf; e != NULL; e++) {
        if (*e == '\0')
            data_end = read_some(ffd, e, sizeof(buf) - (e - buf));
        switch (*e) {
            case '\0':
                return -1;
            case '\n':
                if (h == NULL) rcpt_set_i--;
                else rcpt_set[rcpt_set_i++] = h;
                *e = '\0';
                data = e + 1;
                f_ptr = lseek(ffd, 0, SEEK_CUR);
                e = NULL;
                break;
            case '@':
                h = e + 1;
                break;
            case ',':
                if (h == NULL) rcpt_set_i--;
                else rcpt_set[rcpt_set_i++] = h;
                rcpt_set[rcpt_set_i++] = e + 1;
                *e = '\0';
                h = NULL;
                break;
        }
        if (e == NULL) break;
    }
    printf("%d\n", rcpt_set_i);
    for (int i = 0; i < rcpt_set_i; i++) printf("%s\n", rcpt_set[i]);

    for (char** r = rcpt_set + 2; rcpt_set_i > 2; r += 2, rcpt_set_i -= 2) {
        struct addrinfo *a;

        if (r[0] == NULL) continue;

        /* Socket Setup */
        if (resolve_first_mx(r[1], target_mx_host, sizeof(target_mx_host)) < 0 ||
            getaddrinfo(target_mx_host, port, &ai_hints, &a) != 0 ||
        //if (getaddrinfo("127.0.0.1", port, &ai_hints, &a) != 0 ||
            (fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol)) == -1 ||
            connect(fd, a->ai_addr, a->ai_addrlen) == -1)
        {
            /* Hold entire domain */
            printf("Connect Failed\n");
            goto HOLD_ALL;
            continue;
        }

        freeaddrinfo(a);

        /* Wait for initial greeting */
        if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 220) goto HOLD_ALL;
        RUN_OFF(data_buf);

        /* Send HELO */
        write_all(fd, greet, greet_len);
        if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 250) goto HOLD_ALL;
        RUN_OFF(data_buf);

        /* Reverse path */
        SEND("MAIL FROM:<%s>\r\n", rcpt_set[0]);
        if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 250) goto HOLD_ALL;
        RUN_OFF(data_buf);

        for (int i = r - rcpt_set; i < rcpt_set_i; i += 2) {
            if (r[i] == NULL) continue;
            if (strcmp(r[1], rcpt_set[i + 1]) == 0) {
                SEND("RCPT TO:<%s>\r\n", rcpt_set[i]);

                /* Bounce */
                if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 250)
                    bounce_set[bounce_set_i++] = r[i];

                r[i] = NULL;
                RUN_OFF(data_buf);
            }
        }
        write_all(fd, "DATA\r\n", 6);
        if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 354) goto HOLD_ALL;
        RUN_OFF(data_buf);

        /* Write the entire file */
        write_all(fd, data, data_end - data);
        lseek(ffd, f_ptr, SEEK_SET);
        for (;;) {
            char *end;

            end = read_some(ffd, data_buf, sizeof(data_buf));
            if (end == data_buf) break;
            write_all(fd, data_buf, end - data_buf);
        }
        write_all(fd, "\r\n.\r\n", 5);

        /* Wait for reply */
        if (read_reply_code(fd, data_buf, sizeof(data_buf)) != 250) goto HOLD_ALL;
        RUN_OFF(data_buf);
        continue;
        
HOLD_ALL:
        for (int i = 0; i < rcpt_set_i; i += 2) {
            if (r[i] == NULL) continue;
            if (strcmp(r[1], r[i + 1]) == 0) {
                hold_set[hold_set_i++] = r[i];
                r[i] = NULL;
            }
        }
        continue;
    }
    for (int i = 0; i < hold_set_i; i++) printf("HOLD: %s\n", hold_set[i]);
    for (int i = 0; i < bounce_set_i; i++) printf("BOUNCE: %s\n", bounce_set[i]);
}
