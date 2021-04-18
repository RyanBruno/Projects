#ifndef PROXY_H
#define PROXY_H

#include <stddef.h>

struct proxy_request {
    struct request* req;
};

struct connection *
proxy_connection_create(int, struct connection *, size_t);

#endif /* PROXY_H */
