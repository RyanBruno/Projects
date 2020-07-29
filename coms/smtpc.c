#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netdb.h>

#define READ_SOCKET() { \
    if (read_line() == -1) {\
        close(fd);fprintf(stderr, "Socket Closed!\n"); return -1; } \
    printf("%s\n", buf); } 

#define FORMAT(...) { \
    if ((n = snprintf(buf, sizeof(buf), __VA_ARGS__)) >= sizeof(buf)) { \
        FAIL("snprintf():\n") }}

#define SEND(...) {\
    FORMAT(__VA_ARGS__) \
    if (write(fd, &buf, n) != n) { \
        FAIL("write():\n") }}

#define OK(...) { \
    READ_SOCKET(); \
    if (buf[0] != '2' && buf[0] != '3') { \
        fprintf(stderr, __VA_ARGS__); return -1; }}

#define FAIL(...) { \
    fprintf(stderr, __VA_ARGS__); \
    exit(-1); }

const char* host =          "0.0.0.0";
const char* port =          "2626";

struct addrinfo ai_hints = {
    .ai_flags    = AI_NUMERICSERV,
    .ai_family   = AF_INET,
    .ai_socktype = SOCK_STREAM,
};

int fd;
long int n;
char buf[1024];

int read_line()
{
    int red = 0;

    for (n = 0; ; ) {
        if ((red = read(fd, buf + n, sizeof(buf) - n)) < 1)
            return -1;

        n += red;

        if (buf[n - 1] ==  '\n')
            break;
    }

    buf[--n] = '\0';

    if (buf[n - 1] == '\r')
        buf[--n] = '\0';

    return n;
}

int main()
{
    struct addrinfo *a;

    /* Socket Setup */
    if (getaddrinfo(host, port, &ai_hints, &a) != 0 ||
        (fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol)) == -1 ||
        connect(fd, a->ai_addr, a->ai_addrlen) == -1)
    {
        fprintf(stderr, "Could not connect\n");
        return -1;
    }

    freeaddrinfo(a);

    OK("No Greeting\n")

    SEND("HELO localhost\r\n")
    OK("No HELO\n")

    SEND("MAIL FROM:ryan@localhost\r\n")
    OK("Reverse Path Not Accepted\n")

    SEND("RCPT TO:<ryan@rbruno.com>\r\n")
    OK("Forward Path not accepted\n")


    SEND("DATA\r\n")
    OK("Data\n")
    SEND("Hello My name is joe\n")
    SEND("Gye\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Hello My name is joe\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("Gye\n")
    SEND("\r\n.\r\n")
    OK("Do main queue\n")

    close(fd);

    return 0;
}
