#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"
#include "board_state.h"
#include "eval.h"
#include "movegen.h"

/* Main search function: returns the best move found */
Move search(Board *board, int max_depth, EvalWeights *weights, SearchContext *ctx);

/* Alpha-beta search implementation */
int alpha_beta(Board *board, int alpha, int beta, int depth, bool maximizing_player, EvalWeights *weights, SearchContext *ctx);

#endif /* SEARCH_H */
