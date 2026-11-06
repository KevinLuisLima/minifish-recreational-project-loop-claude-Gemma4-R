#ifndef TT_H
#define TT_H

#include "types.h"

typedef enum {
    TT_NONE,
    TT_EXACT,
    TT_LOWERBOUND,
    TT_UPPERBOUND
} TTNodeType;

typedef struct {
    uint64_t key;
    int depth;
    int value;
    TTNodeType type;
    Move best_move;
} TTNode;

void tt_init(void);
bool tt_probe(uint64_t key, int depth, TTNode *node);
void tt_store(uint64_t key, int depth, int value, TTNodeType type, Move best_move);

#endif /* TT_H */
