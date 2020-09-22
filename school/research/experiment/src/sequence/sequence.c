#include "sequence.h"
#include <string.h>
#include <stdlib.h>

void sequence_init(struct sequence* seq, size_t seq_size) // Size must be > 1
{
    seq->seq_seq = malloc(sizeof(struct sequence_item) * seq_size);
    seq->seq_seq[0].si_no = 0;
    seq->seq_size = seq_size;
    seq->seq_next = 1;
}

void sequence_push(struct sequence* seq, void* si_data)
{
    int i;

    for (i = 0;; i++) {
        if (i > seq->seq_size - 1) {
            size_t nsize;

            seq->seq_size *= 1.5;
            if (!(seq->seq_seq = realloc(seq->seq_seq, sizeof(struct sequence_item) * seq->seq_size)))
                exit(-1);
        }
        if (seq->seq_seq[i].si_no == 0) break;
    }

    seq->seq_seq[i].si_no = seq->seq_next++;
    seq->seq_seq[i].si_data = si_data;
    seq->seq_seq[i + 1].si_no = 0;
}

void* sequence_get(struct sequence* seq, unsigned long si_no)
{
    struct sequence_item* i;

    for (i = seq->seq_seq; i->si_no != 0; i++) {
        if (i->si_no == si_no) return i->si_data;
    }

    return NULL;
}

int sequence_remove(struct sequence* seq, unsigned long si_no)
{
    for (int i = 0;; i++) {
        if (seq->seq_seq[i].si_no == 0) break;
        if (seq->seq_seq[i].si_no == si_no) {
            for (;; i++) {
                if (seq->seq_seq[i].si_no == 0) break;
                memcpy(&(seq->seq_seq[i]), &(seq->seq_seq[i + 1]), sizeof(struct sequence_item));
            }
            return 0;
        }
    }
    return -1;
}
