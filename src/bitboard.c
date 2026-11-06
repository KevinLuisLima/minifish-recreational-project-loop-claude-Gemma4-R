#include "bitboard.h"
#include <string.h>

void board_clear(Board *board) {
    memset(board, 0, sizeof(Board));
    board->en_passant_sq = -1;
}

void board_set_piece(Board *board, int sq, PieceColor piece) {
    if (sq < 0 || sq > 63) return;

    // Remove existing piece from that square first
    board_remove_piece(board, sq);

    set_bit(&board->pieces[piece], sq);

    Color color = (piece < 6) ? WHITE : BLACK;
    set_bit(&board->color_pieces[color], sq);
    set_bit(&board->occupancy, sq);
}

void board_remove_piece(Board *board, int sq) {
    if (sq < 0 || sq > 63) return;

    // Find which piece is at this square
    for (int i = 0; i < 12; i++) {
        if (get_bit(board->pieces[i], sq)) {
            clear_bit(&board->pieces[i], sq);

            Color color = (i < 6) ? WHITE : BLACK;
            clear_bit(&board->color_pieces[color], sq);
            break;
        }
    }
    clear_bit(&board->occupancy, sq);
}

void update_occupancy(Board *board) {
    board->occupancy = board->color_pieces[WHITE] | board->color_pieces[BLACK];
}
