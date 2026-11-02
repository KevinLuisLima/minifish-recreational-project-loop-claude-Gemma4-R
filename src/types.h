#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t Bitboard;

typedef enum {
    WHITE,
    BLACK,
    NONE
} Color;

typedef enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef struct {
    Color color;
    PieceType type;
} Piece;

typedef struct {
    Bitboard pieces[2][6]; // [color][type]
    Bitboard occupancy[3]; // [WHITE, BLACK, ALL]
    Color sideToMove;
    int castlingRights;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
} Board;

#endif
