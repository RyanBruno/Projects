#include <stddef.h>

struct sequence_item {
    unsigned long si_no;
    void* si_data;
};

struct sequence {
    struct sequence_item* seq_seq;
    size_t seq_size;
    unsigned long seq_next;
};

void sequence_init(struct sequence* b, size_t size);

void sequence_push(struct sequence* b, void* data);

void* sequence_get(struct sequence* b, unsigned long seq_no);

int sequence_remove(struct sequence* b, unsigned long seq_no);
