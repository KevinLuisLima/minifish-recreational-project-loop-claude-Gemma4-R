#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

// Funções básicas de manipulação de bitboards
Bitboard get_bit(Bitboard bb, int square);
Bitboard set_bit(Bitboard bb, int square);
Bitboard clear_bit(Bitboard bb, int square);
int count_bits(Bitboard bb);

// Geração de movimentos (simplificada para o esqueleto)
Bitboard get_pawn_attacks(Bitboard bb, Color color);
Bitboard get_knight_attacks(Bitboard bb);
Bitboard get_bishop_attacks(Bitboard bb, Bitboard occupancy);
Bitboard get_rook_attacks(Bitboard bb, Bitboard occupancy);
Bitboard get_queen_attacks(Bitboard bb, Bitboard occupancy);
Bitboard get_king_attacks(Bitboard bb);

#endif
