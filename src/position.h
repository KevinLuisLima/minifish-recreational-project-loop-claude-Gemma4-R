#ifndef POSITION_H
#define POSITION_H

#include "types.h"
#include "bitboard.h"

typedef struct {
    int from;
    int to;
    int promotion; // PieceType or -1
} Move;

typedef enum {
    GAME_ONGOING,
    GAME_CHECKMATE,
    GAME_STALEMATE,
    GAME_FIFTY_MOVE,
    GAME_REPETITION,
    GAME_INSUFFICIENT_MATERIAL,
    GAME_LIMIT_REACHED
} GameStatus;

void init_board(Board* board);
int load_fen(Board* board, const char* fen);
void export_fen(Board* board, char* buffer);

void make_move(Board* board, Move move);
void unmake_move(Board* board, Move move, Board* prev_state);

int is_square_attacked(Board* board, int square, Color attacker);
int is_in_check(Board* board, Color color);

GameStatus check_game_over(Board* board);

#endif
