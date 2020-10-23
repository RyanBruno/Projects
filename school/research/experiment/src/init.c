#include <rpc/rpc.h>
#include <pthread.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "node.h"

/* From config.h */
extern time_t MERGE_RATE;
extern time_t DURATION;
extern int PEERS_LEN;
extern struct peer_node* peers;
extern void* (**threads) (void*);

int main(int argc, char* argv[])
{
    char c;
    node_t node_id;

    while ((c = getopt(argc, argv, ":m:p:d:e:")) != -1) {
        switch (c) {
        case 'm':
            MERGE_RATE = strtol(optarg, NULL, 10);
            break;
        case 'p':
            PEERS_LEN = strtol(optarg, NULL, 10);
            break;
        case 'd':
            DURATION = strtol(optarg, NULL, 10);
            break;
        case ':':
            if (isprint (optopt)) {
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                return 1;
            }
        case '?':
            if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort ();
        }
    }

    if (argc < 2) {
        printf("USAGE: ./node [-m MERGE_RATE] [-p PEERS_LEN] [-d DURATION] NODE_ID\n");
        return -1;
    }

    /* Parse NODEID */
    node_id = strtol(argv[1], NULL, 10);

    /* Some param checking. */
    if (node_id >= PEERS_LEN) {
        printf("main(): NODEID is our of range\n");
        return -1;
    }

    { /* Setup Random */
        unsigned long seed;

        /* Seed rand() with some entropy. */
        if (getentropy(&seed, sizeof(seed)) < 0) {
            fprintf(stderr, "getentropy():\n");
            exit(-1);
        }
        srand(seed);
    }

    /* Create our node's OrSet */
    if (node_init(node_id) < 0)
        return -1;

    /* Register our service */
    if (register_procedure(peers[node_id].peer_prognum)) {
        fprintf(stderr, "register_procedure():\n");
        exit(-1);
    }

    /* Achieves "plugin-like modularity by
     * starting all functions in 'threads'
     * in config.h in a new thread.
     */
    {
        pthread_t thread;

        /* Creates all the threads in 'threads' */
        for (int i = 0; threads[i] != NULL; i++)
        {
            /* Create a thread to adding and removing items */
            if (pthread_create(&thread, NULL,
                        threads[i], NULL))
            {
                fprintf(stderr, "pthread_create():\n");
                exit(-1);
            }
        }
    }

    /* This function never returns. It
     * waits for requests then runs
     * rpc_request.
     */
    svc_run();

    return -1;
}
