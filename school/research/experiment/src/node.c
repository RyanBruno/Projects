/* Represents a logic to bring together all
 * modules into a full functioning node. Its
 * job is to handle incoming merge requests
 * and occasionally send merge request to
 * peers.
 */
#include <rpc/rpc.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "node.h"
#include "ospc/ospc.h"
#include "xdr_orset/xdr_orset.h"

#define VERSION_NUMBER 1

/* Used to build the baseline network
 * information about a single peer.
 */
struct peer_node {
    unsigned short peer_id;     // The node's peer id (right now unused).
    char* peer_host;            // The node's host name (passed in to clnt_create()).
    unsigned long peer_prognum; // The RPC prognum (see man rpc(3)).
};

/* Global OrSet variables */
struct orset os;
struct ospc_context oc;
sem_t os_sem;
struct peer_node peers[10];
int peer_len;
int node_id;

/* Is called by the RPC library when ever a
 * merge request comes in. All it does so
 * far is parse with xdr_orset, merge the
 * remote set with the local one using the
 * latest merge algorithm, free the remote
 * set.
 */
void rpc_merge_request(struct svc_req *req, SVCXPRT *xprt)
{
    struct orset rmt_os;
    int rc;

    /* Parse incoming information */
    if (!svc_getargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: Invalid arguments.\n");

        /* Send an error reply code */
        rc = -1;
        svc_sendreply(xprt, (xdrproc_t) xdr_int, &rc);
        return;
    }

    /* Merge */
    sem_wait(&os_sem);
    ospc_merge(&oc, &rmt_os);
    sem_post(&os_sem);

    /* Cleanup */
    if (!svc_freeargs(xprt, (xdrproc_t) xdr_orset, &rmt_os)) {
        printf("rpc_merge_request: free\n");

        /* Send an error reply code */
        rc = -1;
        svc_sendreply(xprt, (xdrproc_t) xdr_int, &rc);

        /* Exit */
        exit(-1);
        return;
    }

    /* Send a reply code */
    rc = 0;
    svc_sendreply(xprt, (xdrproc_t) xdr_int, &rc);
}

/* Creates an RPC service and registers
 * rpc_merge_request with the port mapper
 * on 'prognum'.
 */
int register_procedure(long int prognum)
{
    int rc;
    SVCXPRT* xprt;

    /* Create a RPC service socket. */
    if ((xprt = svctcp_create(RPC_ANYSOCK, 0, 0)) == NULL) {
        printf("register_procedure: Could not create service.\n");
        return -1;
    }

    /* Un-register any previous registrations
     * on our prognum.
     */
    pmap_unset(prognum, VERSION_NUMBER);

    /* Register rpc_merge_request procedure */
    if (!svc_register(xprt, prognum, VERSION_NUMBER,
                rpc_merge_request, IPPROTO_TCP))
    {
        printf("register_procedure: Could not register.\n");
        return -1;
    }

    return 0;
}

void* client_thread_fn(void* v)
{
    int n = 0;

    sleep(5);
    while (1) {
        int rc;
        struct timeval to;
        CLIENT* client;

        to.tv_sec = 20;

        if (n == node_id) n++;
        if (n >= peer_len) n = 0;


        client = clnt_create(peers[n].peer_host,
                             peers[n].peer_prognum,
                             VERSION_NUMBER,
                             "tcp");

        if (client == NULL) {
            printf("client_thread_fn: clnt_create(3) has failed!\n");
            n++;
            continue;
            exit(-1);
        }

        clnt_call(client, 1,
                 /* Params */
                 (xdrproc_t) xdr_orset, oc.oc_orset,
                 /* Response */
                 (xdrproc_t) xdr_int, &rc,
                 to);

        n++;
        sleep(5);
    }
}

int parse_node_list(char* path, struct peer_node* peer_array, int peer_array_max_len)
{
    int fd;
    char buf[1024];
    int i;
    int p;
    int n;

    if ((fd = open(path, O_RDONLY)) < 0)
        return -1;

    { // Read the entire file
        int red = 0;
        int r;
        while ((r = read(fd, buf + red, 1024 - red))) {
            red += r;
            if (red >= 1024)
                return -4;
        }
    }

    peer_array[0].peer_host = buf;
    for (i = 0, p = 0, n = 0; ; i++) {
        if (buf[i] == '\0')
            break;
        if (buf[i] == ' ') {
            char* endptr;

            if (n++ >= peer_array_max_len)
                return -3;

            buf[i] = '\0';
            peer_array[p].peer_host = strdup(peer_array->peer_host);
            peer_array[p].peer_id = p;
            peer_array[p].peer_prognum = strtol(buf + i + 1, &endptr, 10);

            i += (endptr - (buf + i));
            if (buf[i] != '\n')
                return -2;
            p++; i++;
            if (p > peer_array_max_len)
                return -5;
            peer_array[p].peer_host = buf + i;
        }
    }
    return n;

}

int main(int argc, char* argv[])
{
    int n;

    pthread_t client_thread;

    /* Check args */
    if (argc < 2) {
        printf("USAGE: ./node NODEID\n");
        return -1;
    }

    /* Parse NODEID */
    node_id = strtol(argv[1], NULL, 10);

    /* Parse the node_list.txt file */
    if ((peer_len = parse_node_list("node_list.txt", peers, 10)) < 0) {
        printf("Error parsing node list (%d)\n", peer_len);
        return -1;
    }

    if (node_id >= peer_len) {
        /* TODO */
    }

    /* Create our OrSet */
    orset_create(&os, node_id);
    ospc_wrap(&os, &oc);

    /* Create a semaphore so our two threads
     * do not step on each other.
     */
    if (sem_init(&os_sem, 0, 1)) {
        printf("sem_init():\n");
        exit(-1);
    }

    /* Create a thread to send merge messages */
    if (pthread_create(&client_thread,
                NULL,
                client_thread_fn,
                NULL))
    {
        printf("pthread_create():\n");
        exit(-1);
    }


    /* Register our service */
    if (register_procedure(peers[node_id].peer_prognum)) {
        printf("register_procedure():\n");
        exit(-1);
    }

    svc_run();

    return -1;
}
