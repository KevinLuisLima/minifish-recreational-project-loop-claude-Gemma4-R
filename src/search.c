#include "search.h"
#include "movegen.h"
#include "evaluation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MATE_SCORE 30000

static long long nodes_visited = 0;

void mvv_lva_order(Board* board, Move* moves, int count) {
    int values[] = {0, 100, 320, 330, 500, 900, 20000};
    int scores[1024];

    for (int i = 0; i < count; i++) {
        int from = moves[i].from;
        int to = moves[i].to;

        PieceType attacker = NONE;
        for (int t = 0; t < 6; t++) {
            if (board->pieces[board->sideToMove][t] & (1ULL << from)) {
                attacker = (PieceType)t;
                break;
            }
        }

        PieceType victim = NONE;
        Color opp = (board->sideToMove == WHITE) ? BLACK : WHITE;
        for (int t = 0; t < 6; t++) {
            if (board->pieces[opp][t] & (1ULL << to)) {
                victim = (PieceType)t;
                break;
            }
        }

        if (victim != NONE) {
            scores[i] = values[victim] - values[attacker];
        } else {
            scores[i] = -10000;
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int temp_s = scores[i]; scores[i] = scores[j]; scores[j] = temp_s;
                Move temp_m = moves[i]; moves[i] = moves[j]; moves[j] = temp_m;
            }
        }
    }
}

int quiesce(Board* board, EvaluationEngine* engine, int alpha, int beta) {
    int stand_pat = engine->evaluate(engine, board);
    if (stand_pat >= beta) return beta;
    if (stand_pat > alpha) alpha = stand_pat;

    Move moves[1024];
    int count = generate_pseudo_legal_moves(board, moves);

    for (int i = 0; i < count; i++) {
        int to = moves[i].to;
        Color opp = (board->sideToMove == WHITE) ? BLACK : WHITE;
        if (!(board->occupancy[opp] & (1ULL << to))) continue;

        Board next;
        memcpy(&next, board, sizeof(Board));
        make_move(&next, moves[i]);

        int score = -quiesce(&next, engine, -beta, -alpha);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int negamax(Board* board, EvaluationEngine* engine, int depth, int alpha, int beta, int ply) {
    nodes_visited++;
    if (depth == 0) {
        return quiesce(board, engine, alpha, beta);
    }

    Move moves[1024];
    int count = generate_legal_moves(board, moves);

    if (count == 0) {
        if (is_in_check(board, board->sideToMove)) {
            return -MATE_SCORE + ply;
        }
        return 0;
    }

    int best_score = -1000000;
    for (int i = 0; i < count; i++) {
        Board next;
        memcpy(&next, board, sizeof(Board));
        make_move(&next, moves[i]);

        int score = -negamax(&next, engine, depth - 1, -beta, -alpha, ply + 1);

        if (score > best_score) best_score = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }
    return best_score;
}

SearchResult search_best_move(Board* board, SearchConfig config, SearchHooks hooks, EvaluationEngine* engine) {
    SearchResult result;
    result.best_move = (Move){-1, -1, -1};
    result.score = -1000000;

    nodes_visited = 0;
    int current_best_score = -1000000;

    for (int depth = 1; depth <= config.max_depth; depth++) {
        Move moves[1024];
        int count = generate_legal_moves(board, moves);

        if (count == 0) {
            result.score = is_in_check(board, board->sideToMove) ? -MATE_SCORE : 0;
            return result;
        }
        if (count == 1) {
            result.best_move = moves[0];
            result.score = 0;
            return result;
        }

        hooks.order_moves(board, moves, count);

        int alpha = -1000000;
        int beta = 1000000;

        for (int i = 0; i < count; i++) {
            Board next;
            memcpy(&next, board, sizeof(Board));
            make_move(&next, moves[i]);

            int score = -negamax(&next, engine, depth - 1, -beta, -alpha, 1);

            if (score > current_best_score) {
                current_best_score = score;
                result.best_move = moves[i];
                result.score = score;
            }
            if (score > alpha) alpha = score;
        }

        if (hooks.should_stop && hooks.should_stop(nodes_visited, config.time_limit_ms)) break;
    }

    return result;
}
