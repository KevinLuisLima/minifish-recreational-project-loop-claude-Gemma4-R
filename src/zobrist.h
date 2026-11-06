#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

void zobrist_init(void);
uint64_t zobrist_hash_board(Board *board);
uint64_t get_piece_sq_hash(PieceColor piece, int sq);
uint64_t get_side_hash(void);
uint64_t get_castling_hash(int rights);
uint64_t get_ep_hash(int sq);

#endif /* ZOBRIST_H */
