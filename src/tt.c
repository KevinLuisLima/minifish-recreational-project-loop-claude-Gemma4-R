#include "tt.h"
#include <stdlib.h>
#include <string.h>

#define TT_SIZE (1 << 16) // 65536 entries. Roughly 2MB.
static TTNode tt_table[TT_SIZE];

void tt_init(void) {
    memset(tt_table, 0, sizeof(tt_table));
}

bool tt_probe(uint64_t key, int depth, TTNode *node) {
    int idx = key % TT_SIZE;
    if (tt_table[idx].key == key && tt_table[idx].depth >= depth) {
        *node = tt_table[idx];
        return true;
    }
    return false;
}

void tt_store(uint64_t key, int depth, int value, TTNodeType type, Move best_move) {
    int idx = key % TT_SIZE;
    tt_table[idx].key = key;
    tt_table[idx].depth = depth;
    tt_table[idx].value = value;
    tt_table[idx].type = type;
    tt_table[idx].best_move = best_move;
}
