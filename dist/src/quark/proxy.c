#include <stddef.h>
       #include <sys/socket.h>

#include "proxy.h"
#include "sock.h"
#include "connection.h"


struct connection *
connection_get_drop_candidate(struct connection *connection, size_t nslots);

struct connection *
proxy_connection_create(int insock, struct connection *connection, size_t nslots)
{

	struct connection *c = NULL;
	size_t i;

	/* find vacant connection (i.e. one with no fd assigned to it) */
	for (i = 0; i < nslots; i++) {
		if (connection[i].fd == 0) {
			c = &connection[i];
			break;
		}
	}
	if (i == nslots) {
		/*
		 * all our connection-slots are occupied and the only
		 * way out is to drop another connection, because not
		 * accepting this connection just kicks this can further
		 * down the road (to the next queue_wait()) without
		 * solving anything.
		 *
		 * This may sound bad, but this case can only be hit
		 * either when there's a (D)DoS-attack or a massive
		 * influx of requests. The latter is impossible to solve
		 * at this moment without expanding resources, but the
		 * former has certain characteristics allowing us to
		 * handle this gracefully.
		 *
		 * During an attack (e.g. Slowloris, R-U-Dead-Yet, Slow
		 * Read or just plain flooding) we can not see who is
		 * waiting to be accept()ed.
		 * However, an attacker usually already has many
		 * connections open (while well-behaved clients could
		 * do everything with just one connection using
		 * keep-alive). Inferring a likely attacker-connection
		 * is an educated guess based on which in-address is
		 * occupying the most connection slots. Among those,
		 * connections in early stages (receiving or sending
		 * headers) are preferred over connections in late
		 * stages (sending body).
		 *
		 * This quantitative approach effectively drops malicious
		 * connections while preserving even long-running
		 * benevolent connections like downloads.
		 */
		c = connection_get_drop_candidate(connection, nslots);
		c->res.status = 0;
		connection_log(c);
		connection_reset(c);
	}
    /* TODO DNS */

    /* socket() */
    if ((c->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return NULL;
    }

    /* connect() */
    if (connect(c->fd, (struct sockaddr *)&c->ia,
                (socklen_t) sizeof(c->ia)) < 0) {
        return NULL;
    }

	/* set socket to non-blocking mode */
	if (sock_set_nonblocking(c->fd)) {
		/* we can't allow blocking sockets */
		return NULL;
	}

	return c;
}

/*void
proxy_connection_serve(struct connection *, const struct server *)
{
	enum status s;
	int done;

	switch (c->state) {
	case C_PROXY_SEND_HEADER:
		c->state = C_SEND_HEADERS;

		if ((s = http_send_buf(c->fd, &c->buf))) {
			c->res.status = s;
			goto err;
		}
		if (c->buf.len > 0) {
			return;
		}

		c->state = C_PROXY_RECV;
	case C_PROXY_RECV:
		break;
	}
}*/
