/* Represents some logic to bring together
 * all modules into a full functioning node.
 * Its job is to handle incoming merge
 * requests and occasionally send merges
 * request to peers.
 */
#include "node.h"

#include <stdio.h>
#include <unistd.h>

#include "rpc/rpc.h"

extern time_t MERGE_RATE;
extern int PEERS_LEN;

/* Global Node variables */
struct orset os;
struct ospc_context oc;
sem_t os_sem;

/* Merges a remote set with node's local
 * set.
 */
uint64_t merge_request(struct orset* rmt_os)
{
    uint64_t latest_item;

    /* Merge */
    sem_wait(&os_sem);
    latest_item = ospc_merge(&oc, rmt_os);
    sem_post(&os_sem);

    return latest_item;
}

/* This function is great for stating a
 * node without using the main() function
 * below. It creates the global OrSet and
 * semaphore.
 */
int node_init(node_t node_id, unsigned long prognum)
{
    /* Create our OrSet */
    orset_create(&os, node_id);
    ospc_wrap(&os, &oc);

    /* Create a semaphore so our two threads
     * do not step on each other.
     */
    if (sem_init(&os_sem, 0, 1)) {
        fprintf(stderr, "sem_init():\n");
        return -1;
    }

    /* Init the RPC framework. */
    rpc_init(prognum);

    return 0;
}

/* The client thread occasionally sends
 * merge requests to other peers. It makes
 * sure it does not try to merge with
 * itself based on node_id.
 */
void* client_thread_fn(void* v)
{
    (void)(v);

    for (;;) {
        enum clnt_stat stat;
        uint64_t latest_item;
        int target_peer;

        /* Wait a little */
        sleep(MERGE_RATE);

        /* Pick a peer to merge with. */
        target_peer = rand() % (PEERS_LEN - 1);
        if (target_peer >= os.os_node_id) target_peer++;

        /* Send the RPC. */
        stat = rpc_merge_make_request(target_peer, &latest_item);

        if (stat != RPC_SUCCESS) {
            fprintf(stderr, "client_thread_fn(): clnt_call(%d)\n", stat);
            continue;
        }

        /* Runs the OrSet garbage collector */
        sem_wait(&os_sem);

        /* Updates the latest_key_map for
         * this current peer. If not updated
         * continue.
         */
        if (ospc_touch(&oc, target_peer, latest_item)) {
            /* If updated try to collect some
             * tombstones.
             */
            ospc_collect(&oc);
        }

        sem_post(&os_sem);
    }
}

