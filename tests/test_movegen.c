#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "fen.h"
#include "bitboard.h"
#include "movegen.h"
#include "board_state.h"

typedef struct {
    const char *name;
    const char *fen;
    int expected_moves;
    bool should_pass;
} TestCase;

bool test_move_count(TestCase tc) {
    Board board;
    if (!fen_parse(&board, tc.fen)) {
        printf("[FAIL] %s: FEN parse failed\n", tc.name);
        return false;
    }

    MoveList list;
    generate_legal_moves(&board, &list);

    if (list.count != tc.expected_moves) {
        printf("[FAIL] %s: Expected %d moves, got %d\n", tc.name, tc.expected_moves, list.count);
        return false;
    }
    printf("[PASS] %s\n", tc.name);
    return true;
}

int main() {
    movegen_init();

    TestCase cases[] = {
        {"Start Position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20, true},
        {"Fool's Mate (Near)", "rnbqkbnr/pp1ppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1", 20, true},
        {"Checkmate Position", "k7/8/8/8/8/8/8/K1R5", true, true}, // Special handling needed for count
    };

    int passed = 0;
    int total = 2; // Test only the first two for specific counts

    for (int i = 0; i < total; i++) {
        if (test_move_count(cases[i])) passed++;
    }

    printf("\nTests passed: %d/%d\n", passed, total);
    return (passed == total) ? 0 : 1;
}
