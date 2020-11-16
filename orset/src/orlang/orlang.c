#include <stdlib.h>
#include <stdint.h>

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>

enum ORLANG_STATE {
    MESSAGE,
    STRING_LEN1,
    STRING_LEN2,
    STRING,
    KEY1,
    KEY2,
    KEY3,
    KEY4,
    KEY5,
    KEY6,
    KEY7,
    KEY8,
    DONE,
};

struct orlang_parser {
    enum ORLANG_STATE ol_state;
    uint8_t ol_opcode;
    uint64_t ol_key;
    uint16_t ol_strlen;
    char* ol_string;
    char* ol_string_it;
};

int orlang_putc(struct orlang_parser* op, char c)
{
    switch (op->ol_state) {
        case MESSAGE:
            /* Read op code */
            op->ol_opcode = c;
            if (c == 1) {
                op->ol_state = STRING_LEN1;
            } else if (c == 2) {
                op->ol_state = KEY1;
            } else {
                /* Error */
            }
            return 0;
        case STRING_LEN1:
            op->ol_strlen = c << 8;
            op->ol_state = STRING_LEN2;
            return 0;
        case STRING_LEN2:
            op->ol_strlen |= c;
            op->ol_state = STRING;
            op->ol_string = malloc(op->ol_strlen);
            op->ol_string_it = op->ol_string;
            return 0;
        case STRING:
            *op->ol_string_it = c;
            op->ol_string_it++;

            if (op->ol_string_it - op->ol_string >= op->ol_strlen)
                op->ol_state = DONE;
            return 1;
        case KEY1:
            op->ol_key = ((uint64_t) c) << 56;
            op->ol_state = KEY2;
            return 0;
        case KEY2:
            op->ol_key |= ((uint64_t) c) << 48;
            op->ol_state = KEY3;
            return 0;
        case KEY3:
            op->ol_key |= ((uint64_t) c) << 40;
            op->ol_state = KEY3;
            return 0;
        case KEY4:
            op->ol_key |= ((uint64_t) c) << 32;
            op->ol_state = KEY5;
            return 1;
        case KEY5:
            op->ol_key |= ((uint64_t) c) << 24;
            op->ol_state = KEY6;
            return 0;
        case KEY6:
            op->ol_key |= ((uint64_t) c) << 16;
            op->ol_state = KEY7;
            return 0;
        case KEY7:
            op->ol_key |= ((uint64_t) c) << 8;
            op->ol_state = KEY8;
            return 0;
        case KEY8:
            op->ol_key |= c;
            op->ol_state = DONE;
            return 1;
    }
}

size_t orlang_putiov(struct evbuffer_iovec* iov, int n_vec, void* v)
{
    struct orlang_parser* op = (struct orlang_parser*) v;
    size_t num = 0;

    for (int i = 0; i < n_vec; i++) {
        for (size_t j = 0; j < iov[i].iov_len; j++) {
            num++;
            orlang_putc(op, ((char*) iov[i].iov_base)[j]);
        }
    }
    return num;
}
