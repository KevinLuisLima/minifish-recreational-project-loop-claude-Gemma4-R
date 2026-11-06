#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "bitboard.h"

/* Weight vector structure for evaluation */
typedef struct {
    float material[6];           /* P, N, B, R, Q, K */
    float pst[6][64];           /* Piece-Square Tables for each piece type */
    float mobility_weight;
    float pawn_structure_weight;
} EvalWeights;

/* Evaluation function: returns a value relative to the side to move */
int evaluate(Board *board, EvalWeights *weights);

/* Load weights from binary file */
bool load_weights(EvalWeights *weights, const char *filename);

#endif /* EVAL_H */
