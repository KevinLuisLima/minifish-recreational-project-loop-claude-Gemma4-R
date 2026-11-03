#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"
#include "position.h"

// Generate all pseudo-legal moves for the current side to move
int generate_pseudo_legal_moves(Board* board, Move* moves);

// Generate only legal moves (filtering those that leave king in check)
int generate_legal_moves(Board* board, Move* moves);

// Specialized generators for different pieces
Bitboard get_pawn_moves(Board* board, Bitboard pawns, Color color);
Bitboard get_knight_moves(Board* board, Bitboard knights);
Bitboard get_bishop_moves(Board* board, Bitboard bishops);
Bitboard get_rook_moves(Board* board, Bitboard rooks);
Bitboard get_queen_moves(Board* board, Bitboard queens);
Bitboard get_king_moves(Board* board, Bitboard kings);

#endif
