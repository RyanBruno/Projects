#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

const char*
hello_world(const char* u)
{
    char* res;

    res = (char*) malloc(4096);
    assert(res != NULL);

    snprintf(res, 4096, "HTTP/1.1 200 OK\r\nDate: Mon, 23 May 2005 22:38:34 GMT\r\nContent-Length: %d\r\n\r\nHello world %s", strlen("Hello world ") + strlen(u), u);
    return res;
}
