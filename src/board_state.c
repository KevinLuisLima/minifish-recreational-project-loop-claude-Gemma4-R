#include "board_state.h"
#include "movegen.h"
#include "zobrist.h"
#include <string.h>

void make_move(Board *board, Move move, UndoInfo *undo) {
    // Save state for unmaking
    undo->move = move;
    undo->castling_rights = board->castling_rights;
    undo->en_passant_sq = board->en_passant_sq;
    undo->halfmove_clock = board->halfmove_clock;

    int from = move.from;
    int to = move.to;
    Color side = board->side_to_move;
    Color opponent = (side == WHITE) ? BLACK : WHITE;

    // Find piece being moved
    PieceColor piece = PIECE_NONE;
    for (int i = 0; i < 12; i++) {
        if (get_bit(board->pieces[i], from)) {
            piece = (PieceColor)i;
            break;
        }
    }

    // Capture piece at 'to' square
    undo->captured_piece = PIECE_NONE;
    for (int i = 0; i < 12; i++) {
        if (get_bit(board->pieces[i], to)) {
            undo->captured_piece = (PieceColor)i;
            break;
        }
    }

    // Update Zobrist Hash
    board->hash_key ^= get_piece_sq_hash(piece, from);
    board->hash_key ^= get_piece_sq_hash(piece, to);
    if (undo->captured_piece != PIECE_NONE) {
        board->hash_key ^= get_piece_sq_hash(undo->captured_piece, to);
    }
    board->hash_key ^= get_side_hash();

    // 1. Remove moving piece and captured piece
    board_remove_piece(board, from);
    if (undo->captured_piece != PIECE_NONE) {
        board_remove_piece(board, to);
    }

    // 2. Place moving piece at 'to'
    board_set_piece(board, to, piece);

    // 3. Update game state
    board->side_to_move = opponent;
    board->halfmove_clock++;
    board->fullmove_number = (side == BLACK) ? board->fullmove_number + 1 : board->fullmove_number;
}

void unmake_move(Board *board, UndoInfo undo) {
    int from = undo.move.from;
    int to = undo.move.to;
    Color side = board->side_to_move; // This is the side that JUST moved
    Color opponent = (side == WHITE) ? BLACK : WHITE;

    // Restore piece that moved
    PieceColor piece = PIECE_NONE;
    for (int i = 0; i < 12; i++) {
        if (get_bit(board->pieces[i], to)) {
            piece = (PieceColor)i;
            break;
        }
    }

    // Update Zobrist Hash (reverse of make_move)
    board->hash_key ^= get_piece_sq_hash(piece, to);
    board->hash_key ^= get_piece_sq_hash(piece, from);
    if (undo.captured_piece != PIECE_NONE) {
        board->hash_key ^= get_piece_sq_hash(undo.captured_piece, to);
    }
    board->hash_key ^= get_side_hash();

    board_remove_piece(board, to);
    board_set_piece(board, from, piece);

    // Restore captured piece
    if (undo.captured_piece != PIECE_NONE) {
        board_set_piece(board, to, undo.captured_piece);
    }

    // Restore state
    board->side_to_move = opponent;
    board->castling_rights = undo.castling_rights;
    board->en_passant_sq = undo.en_passant_sq;
    board->halfmove_clock = undo.halfmove_clock;
}
