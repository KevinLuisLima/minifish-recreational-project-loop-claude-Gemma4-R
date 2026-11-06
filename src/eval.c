#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Default Piece-Square Tables (Approximate values) */
static const int default_pst[6][64] = {
    { /* Pawn */
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        0,  0, 10, 10, 10, 10,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0
    },
    { /* Knight */
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,  0,  0,  0,  0, -20, -40,
        -30,  0,  10,  15,  15,  10,  0, -30,
        -30,  5,  15,  20,  20,  15,  5, -30,
        -30,  0,  15,  20,  20,  15,  0, -30,
        -30, -5,  10,  15,  15,  10, -5, -30,
        -40, -20,  0,  5,  5,  0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    },
    { /* Bishop */
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,  0,  0,  0,  0,  0,  0, -10,
        -10,  0,  5,  10,  10,  5,  0, -10,
        -10,  5,  5,  10,  10,  5,  5, -10,
        -10,  0,  10,  10,  10,  10,  0, -10,
        -10,  10,  5,  5,  5,  5,  10, -10,
        -10,  0,  0,  0,  0,  0,  0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    },
    { /* Rook */
        0,  0,  0,  0,  0,  0,  0,  0,
        5,  10,  10,  10,  10,  10,  10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    },
    { /* Queen */
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,  0,  0,  0,  0,  0,  0, -10,
        -10,  0,  10,  10,  10,  10,  0, -10,
        -10,  0,  10,  10,  10,  10,  0, -10,
        -10,  0,  10,  10,  10,  10,  0, -10,
        -10,  0,  10,  10,  10,  10,  0, -10,
        -10,  0,  0,  0,  0,  0,  0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    },
    { /* King */
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -30, -30, -20, -20, -10
    }
};

int evaluate(Board *board, EvalWeights *weights) {
    int score = 0;
    Color side = board->side_to_move;
    Color opponent = (side == WHITE) ? BLACK : WHITE;

    for (int p = 0; p < 6; p++) {
        int piece_idx_white = p;
        int piece_idx_black = p + 6;

        Bitboard white_pieces = board->pieces[piece_idx_white];
        Bitboard black_pieces = board->pieces[piece_idx_black];

        // Material
        while (white_pieces) {
            int sq = __builtin_ctzll(white_pieces);
            score += (int)(weights->material[p] * (side == WHITE ? 1 : -1));
            // PST
            score += (int)(weights->pst[p][sq] * (side == WHITE ? 1 : -1));
            white_pieces &= ~(1ULL << sq);
        }
        while (black_pieces) {
            int sq = __builtin_ctzll(black_pieces);
            score += (int)(weights->material[p] * (side == BLACK ? 1 : -1));
            // PST
            score += (int)(weights->pst[p][sq] * (side == BLACK ? 1 : -1));
            black_pieces &= ~(1ULL << sq);
        }
    }
    return score;
}

bool load_weights(EvalWeights *weights, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open weights file %s. Using defaults.\n", filename);
        // Initialize with defaults
        for (int p = 0; p < 6; p++) {
            weights->material[p] = (p == 0) ? 100 : (p == 1) ? 320 : (p == 2) ? 330 : (p == 3) ? 500 : (p == 4) ? 900 : 0;
            for (int s = 0; s < 64; s++) {
                weights->pst[p][s] = (float)default_pst[p][s];
            }
        }
        return false;
    }

    fread(weights, sizeof(EvalWeights), 1, f);
    fclose(f);
    return true;
}
