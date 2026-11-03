#include "position.h"
#include "bitboard.h"
#include "movegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void init_board(Board* board) {
    memset(board, 0, sizeof(Board));
    board->sideToMove = WHITE;
    board->castlingRights = 15;
    board->enPassantSquare = -1;
    board->halfmoveClock = 0;
    board->fullmoveNumber = 1;

    for (int i = 0; i < 8; i++) {
        board->pieces[WHITE][PAWN] |= (1ULL << (8 + i));
        board->pieces[BLACK][PAWN] |= (1ULL << (48 + i));
    }
    board->pieces[WHITE][ROOK] = (1ULL << 0) | (1ULL << 7);
    board->pieces[WHITE][KNIGHT] = (1ULL << 1) | (1ULL << 6);
    board->pieces[WHITE][BISHOP] = (1ULL << 2) | (1ULL << 5);
    board->pieces[WHITE][QUEEN] = (1ULL << 3);
    board->pieces[WHITE][KING] = (1ULL << 4);

    board->pieces[BLACK][ROOK] = (1ULL << 56) | (1ULL << 63);
    board->pieces[BLACK][KNIGHT] = (1ULL << 57) | (1ULL << 62);
    board->pieces[BLACK][BISHOP] = (1ULL << 58) | (1ULL << 61);
    board->pieces[BLACK][QUEEN] = (1ULL << 59);
    board->pieces[BLACK][KING] = (1ULL << 60);

    board->occupancy[WHITE] = 0;
    board->occupancy[BLACK] = 0;
    for (int t = 0; t < 6; t++) {
        board->occupancy[WHITE] |= board->pieces[WHITE][t];
        board->occupancy[BLACK] |= board->pieces[BLACK][t];
    }
    board->occupancy[NONE] = board->occupancy[WHITE] | board->occupancy[BLACK];
}

int load_fen(Board* board, const char* fen) {
    if (!fen) return 0;
    char buf[256];
    strncpy(buf, fen, 255);
    buf[255] = '\0';

    char* token = strtok(buf, " ");
    if (!token) return 0;

    memset(board, 0, sizeof(Board));
    int rank = 7, file = 0;
    int white_kings = 0, black_kings = 0;

    while (*token) {
        char c = *token++;
        if (c == '/') {
            rank--;
            file = 0;
            if (rank < 0) return 0;
        } else if (c >= '1' && c <= '8') {
            file += (c - '0');
            if (file > 8) return 0;
        } else {
            Color color = (c == 'P' || c == 'N' || c == 'B' || c == 'R' || c == 'Q' || c == 'K') ? WHITE : BLACK;
            PieceType type;
            char lower_c = (c >= 'A' && c <= 'Z') ? (c + 32) : c;
            if (lower_c == 'p') type = PAWN;
            else if (lower_c == 'n') type = KNIGHT;
            else if (lower_c == 'b') type = BISHOP;
            else if (lower_c == 'r') type = ROOK;
            else if (lower_c == 'q') type = QUEEN;
            else if (lower_c == 'k') {
                type = KING;
                if (color == WHITE) white_kings++; else black_kings++;
            } else return 0;

            if (file > 7) return 0;
            int sq = rank * 8 + file;
            board->pieces[color][type] |= (1ULL << sq);
            file++;
        }
    }
    if (file != 8 || rank != 0) return 0;
    if (white_kings != 1 || black_kings != 1) return 0;

    token = strtok(NULL, " ");
    if (!token || (token[0] != 'w' && token[0] != 'b')) return 0;
    board->sideToMove = (token[0] == 'w') ? WHITE : BLACK;

    token = strtok(NULL, " ");
    if (token) {
        if (strchr(token, 'K')) board->castlingRights |= 1;
        if (strchr(token, 'Q')) board->castlingRights |= 2;
        if (strchr(token, 'k')) board->castlingRights |= 4;
        if (strchr(token, 'q')) board->castlingRights |= 8;
    } else {
        board->castlingRights = 0;
    }

    token = strtok(NULL, " ");
    if (token && token[0] != '-') {
        if (token[0] < 'a' || token[0] > 'h' || token[1] < '1' || token[1] > '8') return 0;
        int f = token[0] - 'a';
        int r = 8 - (token[1] - '0');
        board->enPassantSquare = r * 8 + f;
    } else {
        board->enPassantSquare = -1;
    }

    token = strtok(NULL, " ");
    if (token) board->halfmoveClock = atoi(token);
    else board->halfmoveClock = 0;

    token = strtok(NULL, " ");
    if (token) board->fullmoveNumber = atoi(token);
    else board->fullmoveNumber = 1;

    board->occupancy[WHITE] = 0;
    board->occupancy[BLACK] = 0;
    for (int t = 0; t < 6; t++) {
        board->occupancy[WHITE] |= board->pieces[WHITE][t];
        board->occupancy[BLACK] |= board->pieces[BLACK][t];
    }
    board->occupancy[NONE] = board->occupancy[WHITE] | board->occupancy[BLACK];

    return 1;
}

int is_square_attacked(Board* board, int square, Color attacker) {
    Color opposite = (attacker == WHITE) ? BLACK : WHITE;
    Bitboard attacks_from_sq = get_pawn_attacks(1ULL << square, opposite);
    if (board->pieces[attacker][PAWN] & attacks_from_sq) return 1;

    for (int t = 1; t < 6; t++) {
        Bitboard attacks = get_attacks(square, t, attacker, board->occupancy[NONE]);
        if (board->pieces[attacker][t] & attacks) return 1;
    }
    return 0;
}

int is_in_check(Board* board, Color color) {
    int king_sq = -1;
    Bitboard kb = board->pieces[color][KING];
    for (int i = 0; i < 64; i++) {
        if (kb & (1ULL << i)) {
            king_sq = i;
            break;
        }
    }
    if (king_sq == -1) return 0;
    return is_square_attacked(board, king_sq, (color == WHITE) ? BLACK : WHITE);
}

void make_move(Board* board, Move move) {
    Color side = board->sideToMove;
    Color opponent = (side == WHITE) ? BLACK : WHITE;
    int from = move.from;
    int to = move.to;

    PieceType type = NONE;
    for (int t = 0; t < 6; t++) {
        if (board->pieces[side][t] & (1ULL << from)) {
            type = (PieceType)t;
            break;
        }
    }

    if (type == PAWN && to == board->enPassantSquare) {
        int captured_pawn = (side == WHITE) ? to - 8 : to + 8;
        board->pieces[opponent][PAWN] &= ~(1ULL << captured_pawn);
    }

    if (type == KING) {
        if (to - from == 2) {
            int rook_from = (side == WHITE) ? 7 : 63;
            int rook_to = (side == WHITE) ? 5 : 61;
            board->pieces[side][ROOK] &= ~(1ULL << rook_from);
            board->pieces[side][ROOK] |= (1ULL << rook_to);
        } else if (to - from == -2) {
            int rook_from = (side == WHITE) ? 0 : 56;
            int rook_to = (side == WHITE) ? 3 : 59;
            board->pieces[side][ROOK] &= ~(1ULL << rook_from);
            board->pieces[side][ROOK] |= (1ULL << rook_to);
        }
    }

    for (int t = 0; t < 6; t++) {
        if (board->pieces[opponent][t] & (1ULL << to)) {
            if (to == 0) board->castlingRights &= ~2;
            if (to == 7) board->castlingRights &= ~1;
            if (to == 56) board->castlingRights &= ~8;
            if (to == 63) board->castlingRights &= ~4;
            board->pieces[opponent][t] &= ~(1ULL << to);
        }
    }
    board->pieces[side][type] &= ~(1ULL << from);
    board->pieces[side][type] |= (1ULL << to);

    if (move.promotion != -1) {
        board->pieces[side][type] &= ~(1ULL << to);
        board->pieces[side][move.promotion] |= (1ULL << to);
    }

    if (type == KING) board->castlingRights = 0;
    else if (type == ROOK) {
        if (from == 0) board->castlingRights &= ~2;
        if (from == 7) board->castlingRights &= ~1;
        if (from == 56) board->castlingRights &= ~8;
        if (from == 63) board->castlingRights &= ~4;
    }

    board->enPassantSquare = -1;
    if (type == PAWN && (to - from == 16 || to - from == -16)) {
        board->enPassantSquare = (side == WHITE) ? to + 8 : to - 8;
    }

    if (type == PAWN || (board->occupancy[opponent] & (1ULL << to))) {
        board->halfmoveClock = 0;
    } else {
        board->halfmoveClock++;
    }
    if (side == BLACK) board->fullmoveNumber++;

    board->occupancy[WHITE] = 0;
    board->occupancy[BLACK] = 0;
    for (int t = 0; t < 6; t++) {
        board->occupancy[WHITE] |= board->pieces[WHITE][t];
        board->occupancy[BLACK] |= board->pieces[BLACK][t];
    }
    board->occupancy[NONE] = board->occupancy[WHITE] | board->occupancy[BLACK];
    board->sideToMove = opponent;
}

void unmake_move(Board* board, Move move, Board* prev_state) {
    (void)move;
    memcpy(board, prev_state, sizeof(Board));
}

GameStatus check_game_over(Board* board) {
    int white_material = 0, black_material = 0;
    for(int t=0; t<6; t++) {
        white_material += count_bits(board->pieces[WHITE][t]);
        black_material += count_bits(board->pieces[BLACK][t]);
    }
    if (white_material == 1 && black_material == 1) return GAME_INSUFFICIENT_MATERIAL;
    if ((white_material == 2 && black_material == 1) || (white_material == 1 && black_material == 2)) {
        int p_count = 0;
        for(int t=1; t<=2; t++) p_count += count_bits(board->pieces[WHITE][t]) + count_bits(board->pieces[BLACK][t]);
        if (p_count == 1) return GAME_INSUFFICIENT_MATERIAL;
    }

    if (board->halfmoveClock >= 100) return GAME_FIFTY_MOVE;
    if (board->fullmoveNumber > 300) return GAME_LIMIT_REACHED;

    Move moves[1024];
    int count = generate_legal_moves(board, moves);
    if (count == 0) {
        if (is_in_check(board, board->sideToMove)) return GAME_CHECKMATE;
        else return GAME_STALEMATE;
    }

    return GAME_ONGOING;
}
