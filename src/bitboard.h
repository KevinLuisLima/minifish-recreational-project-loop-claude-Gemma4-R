#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

/* Constants for fast access */
#define SQ_A1 0
#define SQ_H8 63

/* Basic Bitboard Operations */
static inline void set_bit(Bitboard *bb, int sq) {
    *bb |= (1ULL << sq);
}

static inline void clear_bit(Bitboard *bb, int sq) {
    *bb &= ~(1ULL << sq);
}

static inline bool get_bit(Bitboard bb, int sq) {
    return (bb & (1ULL << sq)) != 0;
}

/* Board manipulation */
void board_clear(Board *board);
void board_set_piece(Board *board, int sq, PieceColor piece);
void board_remove_piece(Board *board, int sq);
void update_occupancy(Board *board);

#endif /* BITBOARD_H */
