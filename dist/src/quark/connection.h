/* See LICENSE file for copyright and license details. */
#ifndef CONNECTION_H
#define CONNECTION_H

#include "proxy.h"
#include "http.h"
#include "server.h"
#include "util.h"

enum connection_state {
	C_VACANT,
	C_RECV_HEADER,
	C_SEND_HEADER,
	C_SEND_BODY,
	C_PROXY_SEND_HEADER,
	C_PROXY_RECV,
	NUM_CONN_STATES,
};

struct connection {
	enum connection_state state;
	int fd;
	struct sockaddr_storage ia;
	struct request req;
	struct response res;
	struct buffer buf;
	size_t progress;
};

struct connection *connection_accept(int, struct connection *, size_t);
void connection_log(const struct connection *);
void connection_reset(struct connection *);
void connection_serve(struct connection *, const struct server *,
                 struct proxy_request *);

#endif /* CONNECTION_H */
