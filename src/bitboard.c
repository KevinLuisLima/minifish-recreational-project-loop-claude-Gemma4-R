#include "bitboard.h"
#include <stdio.h>

Bitboard get_bit(Bitboard bb, int square) {
    return bb & (1ULL << square);
}

Bitboard set_bit(Bitboard bb, int square) {
    return bb | (1ULL << square);
}

Bitboard clear_bit(Bitboard bb, int square) {
    return bb & ~(1ULL << square);
}

int count_bits(Bitboard bb) {
    return __builtin_popcountll(bb);
}

// Implementações simplificadas para o esqueleto funcional
Bitboard get_pawn_attacks(Bitboard bb, Color color) {
    // Lógica de ataque de peão (simplificada)
    return (color == WHITE) ? (bb << 8) : (bb >> 8);
}

Bitboard get_knight_attacks(Bitboard bb) {
    // Lógica de cavalo (simplificada)
    return (bb << 17) | (bb << 15) | (bb >> 17) | (bb >> 15);
}

Bitboard get_bishop_attacks(Bitboard bb, Bitboard occupancy) {
    return 0; // Implementar sliding window
}

Bitboard get_rook_attacks(Bitboard bb, Bitboard occupancy) {
    return 0; // Implementar sliding window
}

Bitboard get_queen_attacks(Bitboard bb, Bitboard occupancy) {
    return get_bishop_attacks(bb, occupancy) | get_rook_attacks(bb, occupancy);
}

Bitboard get_king_attacks(Bitboard bb) {
    return 0; // Implementar adjacentes
}
