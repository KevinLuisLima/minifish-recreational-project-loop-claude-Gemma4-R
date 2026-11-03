#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/types.h"
#include "src/position.h"
#include "src/movegen.h"
#include "src/bitboard.h"

void test_initial_moves() {
    printf("Testing Initial Position Moves... ");
    Board board;
    init_board(&board);
    Move moves[1024];
    int count = generate_legal_moves(&board, moves);
    if (count != 20) {
        printf("FAIL: Expected 20 moves, got %d\n", count);
        exit(1);
    }
    printf("PASS\n");
}

void test_mate_position() {
    printf("Testing Checkmate... ");
    Board board;
    // "R5k1/5ppp/8/8/8/8/8/4K3 b - - 0 1"
    load_fen(&board, "R5k1/5ppp/8/8/8/8/8/4K3 b - - 0 1");
    Move moves[1024];
    int count = generate_legal_moves(&board, moves);
    if (count != 0) {
        printf("FAIL: Expected 0 moves in mate, got %d\n", count);
        exit(1);
    }
    printf("PASS\n");
}

int main() {
    test_initial_moves();
    test_mate_position();
    printf("All movegen tests passed!\n");
    return 0;
}
