#include "bitboard.h"
#include <stdio.h>
#include <assert.h>

Bitboard get_bit(Bitboard bb, int square) {
    assert(square >= 0 && square <= 63);
    return bb & (1ULL << square);
}

Bitboard set_bit(Bitboard bb, int square) {
    assert(square >= 0 && square <= 63);
    return bb | (1ULL << square);
}

Bitboard clear_bit(Bitboard bb, int square) {
    assert(square >= 0 && square <= 63);
    return bb & ~(1ULL << square);
}

int count_bits(Bitboard bb) {
    return __builtin_popcountll(bb);
}

static const Bitboard FILE_A = 0x0101010101010101ULL;
static const Bitboard FILE_H = 0x8080808080808080ULL;

Bitboard get_pawn_attacks(Bitboard bb, Color color) {
    Bitboard attacks = 0;
    if (color == WHITE) {
        // White pawn attacks: left (<<7, no col h), right (<<9, no col a)
        attacks |= (bb << 7) & ~FILE_H;
        attacks |= (bb << 9) & ~FILE_A;
    } else {
        // Black pawn attacks: left (>>7, no col a), right (>>9, no col h)
        attacks |= (bb >> 7) & ~FILE_A;
        attacks |= (bb >> 9) & ~FILE_H;
    }
    return attacks;
}

Bitboard get_knight_attacks(Bitboard bb) {
    Bitboard attacks = 0;
    for (int i = 0; i < 64; i++) {
        if (bb & (1ULL << i)) {
            int x = i % 8;
            int y = i / 8;
            int dx[] = {1, 2, 2, 1, -1, -2, -2, -1};
            int dy[] = {2, 1, -1, -2, -2, -1, 1, 2};
            for (int j = 0; j < 8; j++) {
                int nx = x + dx[j], ny = y + dy[j];
                if (nx >= 0 && nx <= 7 && ny >= 0 && ny <= 7) {
                    attacks |= (1ULL << (ny * 8 + nx));
                }
            }
        }
    }
    return attacks;
}

static Bitboard slide_attacks(int sq, Bitboard occupancy, int dx, int dy) {
    Bitboard attacks = 0;
    int x = sq % 8;
    int y = sq / 8;
    while (1) {
        x += dx;
        y += dy;
        if (x < 0 || x > 7 || y < 0 || y > 7) break;
        int next_sq = y * 8 + x;
        attacks |= (1ULL << next_sq);
        if (get_bit(occupancy, next_sq)) break;
    }
    return attacks;
}

Bitboard get_bishop_attacks(Bitboard bb, Bitboard occupancy) {
    Bitboard attacks = 0;
    for (int i = 0; i < 64; i++) {
        if (bb & (1ULL << i)) {
            attacks |= slide_attacks(i, occupancy, 1, 1);
            attacks |= slide_attacks(i, occupancy, -1, 1);
            attacks |= slide_attacks(i, occupancy, 1, -1);
            attacks |= slide_attacks(i, occupancy, -1, -1);
        }
    }
    return attacks;
}

Bitboard get_rook_attacks(Bitboard bb, Bitboard occupancy) {
    Bitboard attacks = 0;
    for (int i = 0; i < 64; i++) {
        if (bb & (1ULL << i)) {
            attacks |= slide_attacks(i, occupancy, 1, 0);
            attacks |= slide_attacks(i, occupancy, -1, 0);
            attacks |= slide_attacks(i, occupancy, 0, 1);
            attacks |= slide_attacks(i, occupancy, 0, -1);
        }
    }
    return attacks;
}

Bitboard get_queen_attacks(Bitboard bb, Bitboard occupancy) {
    return get_bishop_attacks(bb, occupancy) | get_rook_attacks(bb, occupancy);
}

Bitboard get_king_attacks(Bitboard bb) {
    Bitboard attacks = 0;
    for (int i = 0; i < 64; i++) {
        if (bb & (1ULL << i)) {
            int x = i % 8;
            int y = i / 8;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx <= 7 && ny >= 0 && ny <= 7) {
                        attacks |= (1ULL << (ny * 8 + nx));
                    }
                }
            }
        }
    }
    return attacks;
}

Bitboard get_attacks(int square, PieceType type, Color color, Bitboard occupancy) {
    Bitboard bb = (1ULL << square);
    switch (type) {
        case PAWN:   return get_pawn_attacks(bb, color);
        case KNIGHT: return get_knight_attacks(bb);
        case BISHOP: return get_bishop_attacks(bb, occupancy);
        case ROOK:   return get_rook_attacks(bb, occupancy);
        case QUEEN:  return get_queen_attacks(bb, occupancy);
        case KING:   return get_king_attacks(bb);
        default:     return 0;
    }
}
