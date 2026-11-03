#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "position.h"
#include "evaluation.h"

typedef struct {
    Move best_move;
    int score;
} SearchResult;

typedef struct {
    int (*evaluate)(EvaluationEngine* engine, Board* board);
    void (*order_moves)(Board* board, Move* moves, int count);
    int (*should_stop)(int nodes, int time_limit_ms);
} SearchHooks;

typedef struct {
    int max_depth;
    int time_limit_ms;
    int nodes_to_check;
} SearchConfig;

SearchResult search_best_move(Board* board, SearchConfig config, SearchHooks hooks, EvaluationEngine* engine);
void mvv_lva_order(Board* board, Move* moves, int count);

#endif
