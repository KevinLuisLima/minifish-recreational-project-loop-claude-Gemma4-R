#ifndef BOARD_STATE_H
#define BOARD_STATE_H

#include "types.h"
#include "bitboard.h"

/* Structure to store enough info to undo a move */
typedef struct {
    Move move;
    PieceColor captured_piece;
    int castling_rights;
    int en_passant_sq;
    int halfmove_clock;
} UndoInfo;

void make_move(Board *board, Move move, UndoInfo *undo);
void unmake_move(Board *board, UndoInfo undo);

#endif /* BOARD_STATE_H */
