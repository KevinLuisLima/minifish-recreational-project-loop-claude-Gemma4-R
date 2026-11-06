#include "search.h"
#include "tt.h"
#include "zobrist.h"
#include <stdio.h>
#include <limits.h>

#define INF 1000000

int quiescence(Board *board, int alpha, int beta, EvalWeights *weights, SearchContext *ctx) {
    int stand_pat = evaluate(board, weights);
    if (stand_pat >= beta) return beta;
    if (stand_pat < alpha) alpha = stand_pat;

    MoveList list;
    generate_legal_moves(board, &list);

    for (int i = 0; i < list.count; i++) {
        // Only search captures in quiescence
        int from = list.moves[i].from;
        int to = list.moves[i].to;
        bool is_capture = false;
        for (int p = 0; p < 12; p++) {
            if (get_bit(board->pieces[p], to)) {
                is_capture = true;
                break;
            }
        }
        if (!is_capture) continue;

        UndoInfo undo;
        make_move(board, list.moves[i], &undo);
        int eval = -quiescence(board, -beta, -alpha, weights, ctx);
        unmake_move(board, undo);

        if (eval > alpha) alpha = eval;
        if (alpha >= beta) return beta;
    }
    return alpha;
}

int alpha_beta(Board *board, int alpha, int beta, int depth, bool maximizing_player, EvalWeights *weights, SearchContext *ctx) {
    ctx->nodes++;

    // TT Probe
    TTNode tt_node;
    if (tt_probe(board->hash_key, depth, &tt_node)) {
        if (tt_node.type == TT_EXACT) return tt_node.value;
        if (tt_node.type == TT_LOWERBOUND) alpha = (tt_node.value > alpha) ? tt_node.value : alpha;
        if (tt_node.type == TT_UPPERBOUND) beta = (tt_node.value < beta) ? tt_node.value : beta;
        if (beta <= alpha) return tt_node.value;
    }

    if (depth == 0) {
        return quiescence(board, alpha, beta, weights, ctx);
    }

    MoveList list;
    generate_legal_moves(board, &list);

    if (list.count == 0) {
        Bitboard king_pos = board->pieces[board->side_to_move == WHITE ? WHITE_KING : BLACK_KING];
        int king_sq = __builtin_ctzll(king_pos);
        if (is_square_attacked(board, king_sq, board->side_to_move == WHITE ? BLACK : WHITE)) {
            return maximizing_player ? -INF + (100 - depth) : INF - (100 - depth);
        }
        return 0;
    }

    int best_val = -INF;
    Move best_move = list.moves[0];
    bool found_pv = false;

    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        make_move(board, list.moves[i], &undo);
        int eval = -alpha_beta(board, -beta, -alpha, depth - 1, !maximizing_player, weights, ctx);
        unmake_move(board, undo);

        if (eval > best_val) {
            best_val = eval;
            best_move = list.moves[i];
            found_pv = true;
        }
        if (eval > alpha) alpha = eval;
        if (beta <= alpha) break;
    }

    // TT Store
    TTNodeType type = TT_NONE;
    if (best_val <= alpha) type = TT_UPPERBOUND;
    else if (best_val >= beta) type = TT_LOWERBOUND;
    else type = TT_EXACT;
    tt_store(board->hash_key, depth, best_val, type, best_move);

    return best_val;
}

Move search(Board *board, int max_depth, EvalWeights *weights, SearchContext *ctx) {
    Move best_move = {0, 0, 0};
    int best_val = -INF;

    MoveList list;
    generate_legal_moves(board, &list);

    if (list.count == 0) return best_move;

    for (int i = 0; i < list.count; i++) {
        UndoInfo undo;
        make_move(board, list.moves[i], &undo);
        int val = -alpha_beta(board, -INF, INF, max_depth - 1, false, weights, ctx);
        unmake_move(board, undo);

        if (val > best_val) {
            best_val = val;
            best_move = list.moves[i];
        }
    }

    return best_move;
}
