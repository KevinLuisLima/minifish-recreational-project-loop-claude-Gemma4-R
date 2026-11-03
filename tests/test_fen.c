#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/types.h"
#include "src/position.h"
#include "src/bitboard.h"

void test_fen_loading() {
    printf("Testing FEN Loading... ");
    Board board;

    // Case 1: Valid FEN with en passant
    const char* fen1 = "rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1";
    if (!load_fen(&board, fen1)) {
        printf("FAIL: load_fen failed for valid FEN\n");
        exit(1);
    }
    if (board.enPassantSquare != 19) {
        printf("FAIL: enPassantSquare should be 19, got %d\n", board.enPassantSquare);
        exit(1);
    }

    // Case 2: Invalid FEN (no kings)
    const char* fen2 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"; // wait, this is valid
    const char* fen_no_king = "8/8/8/8/8/8/8/8 w - - 0 1";
    if (load_fen(&board, fen_no_king)) {
        printf("FAIL: load_fen should reject FEN without kings\n");
        exit(1);
    }

    printf("PASS\n");
}

int main() {
    test_fen_loading();
    printf("All FEN tests passed!\n");
    return 0;
}
