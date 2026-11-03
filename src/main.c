#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "evaluation.h"
#include "bitboard.h"
#include "position.h"
#include "search.h"
#include "movegen.h"
#include "tournament.h"

// Defined search hooks for main
int main_should_stop(int nodes, int time_limit_ms) {
    return nodes > 1000000; // Simple limit for CLI
}

SearchResult search_with_defaults(Board* board, EvaluationEngine* engine) {
    SearchConfig config = {3, 7000, 100000};
    SearchHooks hooks = {
        .evaluate = (int (*)(EvaluationEngine*, Board*))engine->evaluate,
        .order_moves = mvv_lva_order,
        .should_stop = main_should_stop
    };
    return search_best_move(board, config, hooks, engine);
}

// Recursive function to count nodes for perft
long long count_nodes(Board* board, int depth) {
    if (depth == 0) return 1;

    Move moves[1024];
    int count = generate_legal_moves(board, moves);
    if (count == 0) return 0;

    long long total = 0;
    for (int i = 0; i < count; i++) {
        Board next_board;
        memcpy(&next_board, board, sizeof(Board));
        make_move(&next_board, moves[i]);
        total += count_nodes(&next_board, depth - 1);
    }
    return total;
}

void run_perft(const char* fen, int depth) {
    Board board;
    if (!load_fen(&board, fen)) {
        fprintf(stderr, "Erro: FEN invalido.\n");
        return;
    }
    printf("Perft %d for FEN %s...\n", depth, fen);
    long long nodes = count_nodes(&board, depth);
    printf("Nodes: %lld\n", nodes);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando> [argumentos]\n", argv[0]);
        fprintf(stderr, "Comandos:\n");
        fprintf(stderr, "  \"<FEN>\" [pesos]           - Joga um lance\n");
        fprintf(stderr, "  --perft N \"<FEN>\"          - Conta nos de folha\n");
        fprintf(stderr, "  --list-moves \"<FEN>\"       - Lista lances legais\n");
        fprintf(stderr, "  --tournament <pesos...>    - Roda torneio\n");
        fprintf(stderr, "  --basic                    - Usa motor basico\n");
        return 1;
    }

    if (strcmp(argv[1], "--perft") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s --perft N \"<FEN>\"\n", argv[0]);
            return 1;
        }
        run_perft(argv[3], atoi(argv[2]));
        return 0;
    }

    if (strcmp(argv[1], "--list-moves") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Uso: %s --list-moves \"<FEN>\"\n", argv[0]);
            return 1;
        }
        Board board;
        if (!load_fen(&board, argv[2])) return 1;
        Move moves[1024];
        int count = generate_legal_moves(&board, moves);
        for (int i = 0; i < count; i++) {
            printf("%c%d%c%d%s\n", 'a'+(moves[i].from%8), (moves[i].from/8)+1, 'a'+(moves[i].to%8), (moves[i].to/8)+1,
                   (moves[i].promotion == 1 ? "q" : moves[i].promotion == 2 ? "r" : moves[i].promotion == 3 ? "b" : moves[i].promotion == 4 ? "n" : ""));
        }
        return 0;
    }

    if (strcmp(argv[1], "--tournament") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Uso: %s --tournament <model1.bin> <model2.bin> ...\n", argv[0]);
            return 1;
        }
        int num_models = argc - 2;
        const char** files = (const char**)&argv[2];
        JudgeConfig config = {100, 5, "Internal-Judge"};
        run_tournament(num_models, files, config);
        return 0;
    }

    if (strcmp(argv[1], "--basic") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Uso: %s --basic \"<FEN>\"\n", argv[0]);
            return 1;
        }
        Board board;
        if (!load_fen(&board, argv[2])) return 1;
        EvaluationEngine* engine = create_basic_engine();
        SearchResult res = search_with_defaults(&board, engine);
        if (res.best_move.from == -1) {
            fprintf(stderr, "No legal moves\n");
            destroy_engine(engine);
            return 1;
        }
        printf("%c%d%c%d%s\n", 'a'+(res.best_move.from%8), (res.best_move.from/8)+1, 'a'+(res.best_move.to%8), (res.best_move.to/8)+1,
               (res.best_move.promotion == 1 ? "q" : res.best_move.promotion == 2 ? "r" : res.best_move.promotion == 3 ? "b" : res.best_move.promotion == 4 ? "n" : ""));
        destroy_engine(engine);
        return 0;
    }

    Board board;
    if (!load_fen(&board, argv[1])) {
        fprintf(stderr, "FEN invalido.\n");
        return 1;
    }

    EvaluationEngine* engine = create_hf_engine();
    if (argc >= 3) {
        if (engine->load_weights(engine, argv[2]) != 0) {
            fprintf(stderr, "Erro ao carregar pesos: %s\n", argv[2]);
            destroy_engine(engine);
            return 1;
        }
    } else {
        fprintf(stderr, "Aviso: Sem pesos informados. Usando motor basico.\n");
        destroy_engine(engine);
        engine = create_basic_engine();
    }

    SearchResult res = search_with_defaults(&board, engine);

    if (res.best_move.from == -1) {
        fprintf(stderr, "No legal moves\n");
        destroy_engine(engine);
        return 1;
    } else {
        printf("%c%d%c%d%s\n", 'a'+(res.best_move.from%8), (res.best_move.from/8)+1, 'a'+(res.best_move.to%8), (res.best_move.to/8)+1,
               (res.best_move.promotion == 1 ? "q" : res.best_move.promotion == 2 ? "r" : res.best_move.promotion == 3 ? "b" : res.best_move.promotion == 4 ? "n" : ""));
    }

    destroy_engine(engine);
    return 0;
}
