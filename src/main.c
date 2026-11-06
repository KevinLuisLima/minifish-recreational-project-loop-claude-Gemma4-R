#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "fen.h"
#include "bitboard.h"
#include "movegen.h"
#include "board_state.h"
#include "eval.h"
#include "search.h"
#include "zobrist.h"
#include "tt.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <fen> <weights_path>\n", argv[0]);
        return 1;
    }

    const char *fen = argv[1];
    const char *weights_path = argv[2];

    movegen_init();
    zobrist_init();
    tt_init();

    Board board;
    if (!fen_parse(&board, fen)) {
        fprintf(stderr, "Invalid FEN\n");
        return 1;
    }
    board.hash_key = zobrist_hash_board(&board);

    EvalWeights weights;
    load_weights(&weights, weights_path);

    SearchContext ctx = {0};
    int depth = 3;

    Move best = search(&board, depth, &weights, &ctx);

    // Convert move indices to UCI notation (e.g., 8 -> 16 as "a2a3")
    char uci_move[10];
    char files[] = "abcdefgh";
    char ranks[] = "12345678";

    int from_file = best.from % 8;
    int from_rank = best.from / 8;
    int to_file = best.to % 8;
    int to_rank = best.to / 8;

    sprintf(uci_move, "%c%c%c%c", files[from_file], ranks[from_rank], files[to_file], ranks[to_rank]);
    printf("BEST_MOVE: %s\n", uci_move);

    return 0;
}
