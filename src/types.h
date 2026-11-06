#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Board Representation */
typedef uint64_t Bitboard;

typedef enum {
    WHITE,
    BLACK,
    SIDE_NONE
} Color;

typedef enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef enum {
    WHITE_PAWN = 0, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    PIECE_NONE
} PieceColor;

typedef struct {
    Bitboard pieces[12];          /* One bitboard for each piece-color combination */
    Bitboard color_pieces[2];     /* Combined bitboards for white and black */
    Bitboard occupancy;           /* All pieces on board */

    Color side_to_move;
    int castling_rights;          /* Binary flags for KQkq */
    int en_passant_sq;            /* -1 if none, else square index 0-63 */
    int halfmove_clock;
    int fullmove_number;
    uint64_t hash_key;            /* Zobrist key for the current position */
} Board;

/* Move Representation */
typedef struct {
    int from;
    int to;
    int flags;                    /* Promotion, castling, en passant, etc. */
} Move;

/* Context for Search (avoiding global mutable state as per spec) */
typedef struct {
    int nodes;
    int depth;
    int time_limit_ms;
    uint64_t start_time;
    bool stop_search;
    /* Transposition table would be added here */
} SearchContext;

#endif /* TYPES_H */
