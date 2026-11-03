#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

// Basic bit manipulation
Bitboard get_bit(Bitboard bb, int square);
Bitboard set_bit(Bitboard bb, int square);
Bitboard clear_bit(Bitboard bb, int square);
int count_bits(Bitboard bb);

// Attack generation
Bitboard get_pawn_attacks(Bitboard bb, Color color);
Bitboard get_knight_attacks(Bitboard bb);
Bitboard get_king_attacks(Bitboard bb);
Bitboard get_bishop_attacks(Bitboard bb, Bitboard occupancy);
Bitboard get_rook_attacks(Bitboard bb, Bitboard occupancy);
Bitboard get_queen_attacks(Bitboard bb, Bitboard occupancy);

// Helper to get all attacks for a square
Bitboard get_attacks(int square, PieceType type, Color color, Bitboard occupancy);

#endif
