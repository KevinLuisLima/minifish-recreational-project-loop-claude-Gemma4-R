#include "fen.h"
#include "bitboard.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static int fen_char_to_piece(char c) {
    switch (c) {
        case 'P': return WHITE_PAWN;
        case 'N': return WHITE_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'R': return WHITE_ROOK;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;
        case 'p': return BLACK_PAWN;
        case 'n': return BLACK_KNIGHT;
        case 'b': return BLACK_BISHOP;
        case 'r': return BLACK_ROOK;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;
        default: return -1;
    }
}

bool fen_parse(Board *board, const char *fen) {
    board_clear(board);

    const char *ptr = fen;
    int rank = 7;
    int file = 0;

    // 1. Piece Placement
    while (*ptr && *ptr != ' ') {
        if (*ptr == '/') {
            rank--;
            file = 0;
        } else if (isdigit(*ptr)) {
            file += (*ptr - '0');
        } else {
            int piece = fen_char_to_piece(*ptr);
            if (piece == -1) return false;

            // Standard FEN is read from Rank 8 (top) down to Rank 1 (bottom).
            // Our Board indices: A1 = 0, H8 = 63.
            // Top rank (Rank 8) is rank index 7.
            // bottom rank (Rank 1) is rank index 0.
            int sq = rank * 8 + file;
            board_set_piece(board, sq, piece);
            file++;
        }
        ptr++;
    }

    if (*ptr == ' ') ptr++;

    // 2. Active Color
    if (*ptr == 'w') {
        board->side_to_move = WHITE;
        ptr++;
    } else if (*ptr == 'b') {
        board->side_to_move = BLACK;
        ptr++;
    } else {
        return false;
    }
    if (*ptr == ' ') ptr++;

    // 3. Castling Rights
    board->castling_rights = 0;
    while (*ptr && *ptr != ' ') {
        if (*ptr == 'K') board->castling_rights |= (1 << 0);
        if (*ptr == 'Q') board->castling_rights |= (1 << 1);
        if (*ptr == 'k') board->castling_rights |= (1 << 2);
        if (*ptr == 'q') board->castling_rights |= (1 << 3);
        ptr++;
    }
    if (*ptr == ' ') ptr++;

    // 4. En Passant
    board->en_passant_sq = -1;
    if (*ptr && *ptr != '-') {
        int file_ep = ptr[0] - 'a';
        int rank_ep = ptr[1] - '1';
        board->en_passant_sq = rank_ep * 8 + file_ep;
        ptr += 2;
    } else if (*ptr == '-') {
        ptr++;
    }
    if (*ptr == ' ') ptr++;

    // 5. Halfmove Clock
    if (*ptr) {
        board->halfmove_clock = atoi(ptr);
        while (*ptr && isdigit(*ptr)) ptr++;
    }

    if (*ptr == ' ') ptr++;

    // 6. Fullmove Number
    if (*ptr) {
        board->fullmove_number = atoi(ptr);
    }

    return true;
}
