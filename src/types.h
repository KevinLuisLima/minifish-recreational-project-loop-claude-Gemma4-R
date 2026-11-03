#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t Bitboard;

typedef enum {
    WHITE = 0,
    BLACK = 1,
    NONE = 2
} Color;

typedef enum {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
} PieceType;

typedef struct {
    Color color;
    PieceType type;
} Piece;

typedef struct {
    Bitboard pieces[2][6]; // [color][type]
    Bitboard occupancy[3]; // [WHITE, BLACK, ALL]
    Color sideToMove;
    int castlingRights;    // Binary: 1: WK, 2: WQ, 4: BK, 8: BQ
    int enPassantSquare;    // -1 if none, else square index 0-63
    int halfmoveClock;
    int fullmoveNumber;
} Board;

#define SQUARE_A1 0
#define SQUARE_H8 63

#endif
