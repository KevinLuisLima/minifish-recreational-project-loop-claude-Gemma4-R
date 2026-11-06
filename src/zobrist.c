#include "zobrist.h"
#include <stdlib.h>

static uint64_t piece_sq_hashes[12][64];
static uint64_t side_hash;
static uint64_t castling_hashes[16];
static uint64_t ep_hashes[64];

static uint64_t random_u64(void) {
    return ((uint64_t)rand() << 32) | (uint64_t)rand();
}

void zobrist_init(void) {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            piece_sq_hashes[i][j] = random_u64();
        }
    }
    side_hash = random_u64();
    for (int i = 0; i < 16; i++) castling_hashes[i] = random_u64();
    for (int i = 0; i < 64; i++) ep_hashes[i] = random_u64();
}

uint64_t zobrist_hash_board(Board *board) {
    uint64_t h = 0;
    for (int i = 0; i < 12; i++) {
        Bitboard bb = board->pieces[i];
        while (bb) {
            int sq = __builtin_ctzll(bb);
            h ^= piece_sq_hashes[i][sq];
            bb &= ~(1ULL << sq);
        }
    }
    if (board->side_to_move == BLACK) h ^= side_hash;
    h ^= castling_hashes[board->castling_rights];
    if (board->en_passant_sq != -1) {
        h ^= ep_hashes[board->en_passant_sq];
    }
    return h;
}

uint64_t get_piece_sq_hash(PieceColor piece, int sq) {
    return piece_sq_hashes[piece][sq];
}

uint64_t get_side_hash(void) {
    return side_hash;
}

uint64_t get_castling_hash(int rights) {
    return castling_hashes[rights];
}

uint64_t get_ep_hash(int sq) {
    return ep_hashes[sq];
}
