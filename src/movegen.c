#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include <stdio.h>
#include <string.h>

int generate_pseudo_legal_moves(Board* board, Move* moves) {
    int count = 0;
    Color side = board->sideToMove;
    Bitboard my_pieces = board->occupancy[side];
    Bitboard opp_pieces = board->occupancy[side == WHITE ? BLACK : WHITE];
    Bitboard all_pieces = board->occupancy[NONE];

    for (int sq = 0; sq < 64; sq++) {
        if (!(my_pieces & (1ULL << sq))) continue;

        PieceType type = NONE;
        for (int t = 0; t < 6; t++) {
            if (board->pieces[side][t] & (1ULL << sq)) {
                type = (PieceType)t;
                break;
            }
        }

        if (type == PAWN) {
            int dir = (side == WHITE) ? 8 : -8;
            int start_rank = (side == WHITE) ? 8 : 48;

            // 1. Diagonal Captures (Only on opponent pieces or en passant)
            Bitboard pawn_attacks = get_pawn_attacks(1ULL << sq, side);
            for (int target = 0; target < 64; target++) {
                if (pawn_attacks & (1ULL << target)) {
                    if ((opp_pieces & (1ULL << target)) || target == board->enPassantSquare) {
                        moves[count].from = sq;
                        moves[count].to = target;
                        // Promotion
                        if ((side == WHITE && target >= 56) || (side == BLACK && target <= 7)) {
                            // In a real generator, we'd generate 4 moves. For now, we'll mark it.
                            // To be strict with Spec 6.14:
                            for (int p = 1; p <= 4; p++) {
                                moves[count].from = sq;
                                moves[count].to = target;
                                moves[count].promotion = p;
                                count++;
                            }
                            continue;
                        }
                        moves[count].promotion = -1;
                        count++;
                    }
                }
            }

            // 2. Advances (Only to empty squares)
            int next_sq = sq + dir;
            if (next_sq >= 0 && next_sq < 64 && !(all_pieces & (1ULL << next_sq))) {
                if ((side == WHITE && next_sq >= 56) || (side == BLACK && next_sq <= 7)) {
                    for (int p = 1; p <= 4; p++) {
                        moves[count].from = sq;
                        moves[count].to = next_sq;
                        moves[count].promotion = p;
                        count++;
                    }
                } else {
                    moves[count].from = sq;
                    moves[count].to = next_sq;
                    moves[count].promotion = -1;
                    count++;
                }

                // Double advance
                if (sq >= start_rank && sq < start_rank + 8) {
                    int double_sq = sq + 2 * dir;
                    if (double_sq >= 0 && double_sq < 64 && !(all_pieces & (1ULL << double_sq))) {
                        moves[count].from = sq;
                        moves[count].to = double_sq;
                        moves[count].promotion = -1;
                        count++;
                    }
                }
            }
        } else {
            // Other pieces
            Bitboard attacks = get_attacks(sq, type, side, all_pieces);
            attacks &= ~my_pieces; // Cannot capture own pieces

            for (int target = 0; target < 64; target++) {
                if (attacks & (1ULL << target)) {
                    moves[count].from = sq;
                    moves[count].to = target;
                    moves[count].promotion = -1;
                    count++;
                }
            }
        }
    }
    return count;
}

int generate_legal_moves(Board* board, Move* moves) {
    Move pseudo[1024];
    int pseudo_count = generate_pseudo_legal_moves(board, pseudo);
    int legal_count = 0;

    for (int i = 0; i < pseudo_count; i++) {
        Board temp;
        memcpy(&temp, board, sizeof(Board));
        make_move(&temp, pseudo[i]);
        if (!is_in_check(&temp, board->sideToMove)) {
            moves[legal_count++] = pseudo[i];
        }
    }
    return legal_count;
}
