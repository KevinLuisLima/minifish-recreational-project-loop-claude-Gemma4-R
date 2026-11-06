#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "bitboard.h"
#include "board_state.h"

/* Move generation structures */
typedef struct {
    Move moves[256];
    int count;
} MoveList;

/* Initialization of pre-calculated tables */
void movegen_init(void);

/* Move Generation Functions */
void generate_pseudo_legal_moves(Board *board, MoveList *list);
bool is_square_attacked(Board *board, int sq, Color attacker_color);
void generate_legal_moves(Board *board, MoveList *list);

/* Helper functions for specific piece types */
Bitboard get_knight_attacks(int sq);
Bitboard get_king_attacks(int sq);
Bitboard get_rook_attacks(int sq, Bitboard occupancy);
Bitboard get_bishop_attacks(int sq, Bitboard occupancy);

#endif /* MOVEGEN_H */
