#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/types.h"
#include "src/bitboard.h"
#include "src/position.h"
#include "src/movegen.h"
#include "src/evaluation.h"
#include "src/search.h"

void test_pawn_attacks() {
    printf("Testing Pawn Attacks... ");
    Board board;
    init_board(&board);

    // Case 1: White pawn a2 (sq 8) attacks b3 (sq 17)
    Bitboard a2 = (1ULL << 8);
    Bitboard attacks_a2 = get_pawn_attacks(a2, WHITE);
    if (!(attacks_a2 & (1ULL << 17)) || count_bits(attacks_a2) != 1) {
        printf("FAIL: a2 attacks\n");
        exit(1);
    }

    // Case 2: White pawn h2 (sq 15) attacks g3 (sq 22)
    Bitboard h2 = (1ULL << 15);
    Bitboard attacks_h2 = get_pawn_attacks(h2, WHITE);
    if (!(attacks_h2 & (1ULL << 22)) || count_bits(attacks_h2) != 1) {
        printf("FAIL: h2 attacks\n");
        exit(1);
    }

    // Case 3: Black pawn h7 (sq 62) attacks g6 (sq 46)
    Bitboard h7 = (1ULL << 62);
    Bitboard attacks_h7 = get_pawn_attacks(h7, BLACK);
    if (!(attacks_h7 & (1ULL << 46)) || count_bits(attacks_h7) != 1) {
        printf("FAIL: h7 attacks\n");
        exit(1);
    }

    printf("PASS\n");
}

void test_knight_attacks() {
    printf("Testing Knight Attacks... ");
    Bitboard a1 = (1ULL << 0);
    Bitboard attacks_a1 = get_knight_attacks(a1);
    // a1 attacks b3 (17) and c2 (10)
    if (!(attacks_a1 & (1ULL << 17)) || !(attacks_a1 & (1ULL << 10)) || count_bits(attacks_a1) != 2) {
        printf("FAIL: a1 attacks\n");
        exit(1);
    }
    printf("PASS\n");
}

void test_rook_attacks() {
    printf("Testing Rook Attacks... ");
    Board board;
    init_board(&board);
    // Rook on d4 (sq 27), blocker on d6 (sq 43)
    board.occupancy[NONE] = (1ULL << 27) | (1ULL << 43);
    Bitboard rook_bb = (1ULL << 27);
    Bitboard attacks = get_rook_attacks(rook_bb, board.occupancy[NONE]);
    // Should reach d5 (35) and d6 (43), but not d7 (51)
    if (!(attacks & (1ULL << 35)) || !(attacks & (1ULL << 43)) || (attacks & (1ULL << 51))) {
        printf("FAIL: rook blocking\n");
        exit(1);
    }
    printf("PASS\n");
}

int main() {
    test_pawn_attacks();
    test_knight_attacks();
    test_rook_attacks();
    printf("All attack tests passed!\n");
    return 0;
}
