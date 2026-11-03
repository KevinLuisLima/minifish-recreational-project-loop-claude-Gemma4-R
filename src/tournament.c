#include "tournament.h"
#include "search.h"
#include "movegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static float REF_WEIGHTS[393];

void init_judge_weights() {
    memset(REF_WEIGHTS, 0, sizeof(REF_WEIGHTS));
    float mat[] = {100.0f, 320.0f, 330.0f, 500.0f, 900.0f};
    for(int i=0; i<5; i++) REF_WEIGHTS[i] = mat[i];
}

int judge_evaluate(EvaluationEngine* self, Board* board) {
    (void)self;
    int score = 0;
    for (int type = 0; type < 5; type++) {
        int white_count = count_bits(board->pieces[WHITE][type]);
        int black_count = count_bits(board->pieces[BLACK][type]);
        score += (int)((white_count - black_count) * REF_WEIGHTS[type]);
    }
    return (board->sideToMove == WHITE) ? score : -score;
}

void run_tournament(int num_models, const char** weight_files, JudgeConfig config) {
    init_judge_weights();
    ModelStats stats[num_models];
    memset(stats, 0, sizeof(stats));

    EvaluationEngine* judge_engine = create_hf_engine();
    judge_engine->evaluate = judge_evaluate;
    judge_engine->weights = REF_WEIGHTS;

    SearchConfig s_config = {3, 7000, 100000};
    SearchHooks hooks = {
        .evaluate = judge_evaluate,
        .order_moves = mvv_lva_order,
        .should_stop = NULL // search_best_move handles NULL as "don't stop"
    };

    for (int i = 0; i < num_models; i++) {
        EvaluationEngine* m1 = create_hf_engine();
        if (m1->load_weights(m1, weight_files[i]) != 0) {
            fprintf(stderr, "Erro ao carregar modelo %s\n", weight_files[i]);
            continue;
        }
        strncpy(stats[i].model_name, weight_files[i], 31);
        stats[i].origin = m1->origin;

        for (int j = 0; j < num_models; j++) {
            if (i == j) continue;
            EvaluationEngine* m2 = create_hf_engine();
            m2->load_weights(m2, weight_files[j]);

            Board board;
            init_board(&board);
            int moves_count = 0;

            while (1) {
                if (check_game_over(&board) != GAME_ONGOING) break;
                if (moves_count >= 600) break;

                Color side = board.sideToMove;
                EvaluationEngine* current_engine = (side == WHITE) ? m1 : m2;

                // Judge evaluation
                SearchResult judge_res = search_best_move(&board, s_config, hooks, judge_engine);
                int v_best = judge_res.score;

                // Model search
                SearchHooks model_hooks = {
                    .evaluate = (int (*)(EvaluationEngine*, Board*))current_engine->evaluate,
                    .order_moves = mvv_lva_order,
                    .should_stop = NULL
                };
                SearchResult model_res = search_best_move(&board, s_config, model_hooks, current_engine);
                Move move = model_res.best_move;

                // Judge evaluates played move
                Board next_board;
                memcpy(&next_board, &board, sizeof(Board));
                make_move(&next_board, move);
                int v_played = judge_evaluate(judge_engine, &next_board);

                // Sub-optimal check
                Move legal_moves[1024];
                int l_count = generate_legal_moves(&board, legal_moves);
                if (l_count > 1) {
                    stats[i].total_legal_games++; // Use as a counter for moves
                    if ((v_best - v_played) > config.threshold) {
                        if (side == WHITE) stats[i].suboptimal_moves++;
                        else stats[j].suboptimal_moves++;
                    }
                }

                make_move(&board, move);
                moves_count++;
            }

            if (board.sideToMove == WHITE) stats[i].wins++; else stats[j].wins++;
            destroy_engine(m2);
        }
        destroy_engine(m1);
    }

    printf("\n--- Tournament Results ---\n");
    printf("Judge: %s | T: %d | Depth: %d\n", config.judge_type, config.threshold, config.judge_depth);
    printf("--------------------------------------------------------------------------------\n");
    printf("%-15s | %-10s | %-10s | %-10s | %-10s | %-10s\n", "Model", "Winrate", "W/D/L", "Played", "Sub-opt", "Rate");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < num_models; i++) {
        float winrate = (float)stats[i].wins / (num_models - 1);
        float sub_rate = (stats[i].total_legal_games > 0) ?
                         (float)stats[i].suboptimal_moves / stats[i].total_legal_games : 0;

        printf("%-15s | %-10.2f%% | %-10d/%d/%d | %-10d | %-10d | %-10.4f %s\n",
               stats[i].model_name, winrate * 100, stats[i].wins, stats[i].draws, stats[i].losses,
               stats[i].moves_played, stats[i].suboptimal_moves, sub_rate,
               stats[i].origin == 1 ? "[SYNTHETIC]" : "");
    }
    printf("--------------------------------------------------------------------------------\n");

    destroy_engine(judge_engine);
}
