#include "movegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Pre-calculated attack tables */
static Bitboard knight_attacks[64];
static Bitboard king_attacks[64];

void movegen_init(void) {
    for (int sq = 0; sq < 64; sq++) {
        Bitboard attacks = 0;
        // Knight moves
        int dx[] = {1, 2, 2, 1, -1, -2, -2, -1};
        int dy[] = {2, 1, -1, -2, -2, -1, 1, 2};
        for (int i = 0; i < 8; i++) {
            int nx = (sq % 8) + dx[i];
            int ny = (sq / 8) + dy[i];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                attacks |= (1ULL << (ny * 8 + nx));
            }
        }
        knight_attacks[sq] = attacks;

        // King moves
        attacks = 0;
        for (int dx_k = -1; dx_k <= 1; dx_k++) {
            for (int dy_k = -1; dy_k <= 1; dy_k++) {
                if (dx_k == 0 && dy_k == 0) continue;
                int nx = (sq % 8) + dx_k;
                int ny = (sq / 8) + dy_k;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    attacks |= (1ULL << (ny * 8 + nx));
                }
            }
        }
        king_attacks[sq] = attacks;
    }
}

Bitboard get_knight_attacks(int sq) {
    return knight_attacks[sq];
}

Bitboard get_king_attacks(int sq) {
    return king_attacks[sq];
}

/* Simple sliding attack implementation (not magic bitboards yet to keep it lean) */
Bitboard get_rook_attacks(int sq, Bitboard occupancy) {
    Bitboard attacks = 0;
    int file = sq % 8;
    int rank = sq / 8;

    // North
    for (int s = sq + 8; s < 64; s += 8) {
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // South
    for (int s = sq - 8; s >= 0; s -= 8) {
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // East
    for (int s = sq + 1; s < (rank + 1) * 8; s++) {
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // West
    for (int s = sq - 1; s >= rank * 8; s--) {
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    return attacks;
}

Bitboard get_bishop_attacks(int sq, Bitboard occupancy) {
    Bitboard attacks = 0;
    int rank = sq / 8;
    int file = sq % 8;

    // NE
    for (int s = sq + 9; s < 64; s += 9) {
        if ((s % 8) <= file) break; // Wraps around
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // NW
    for (int s = sq + 7; s < 64; s += 7) {
        if ((s % 8) >= file) break; // Wraps around
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // SE
    for (int s = sq - 7; s >= 0; s -= 7) {
        if ((s % 8) >= file) break; // Wraps around
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    // SW
    for (int s = sq - 9; s >= 0; s -= 9) {
        if ((s % 8) <= file) break; // Wraps around
        attacks |= (1ULL << s);
        if (occupancy & (1ULL << s)) break;
    }
    return attacks;
}

/* Pseudo-legal move generation */
void generate_pseudo_legal_moves(Board *board, MoveList *list) {
    list->count = 0;
    Color side = board->side_to_move;
    Bitboard my_pieces = board->color_pieces[side];

    for (int sq = 0; sq < 64; sq++) {
        if (!get_bit(my_pieces, sq)) continue;

        PieceType type = PIECE_NONE;
        for (int p = 0; p < 6; p++) {
            int piece_idx = (side == WHITE) ? p : p + 6;
            if (get_bit(board->pieces[piece_idx], sq)) {
                type = (PieceType)p;
                break;
            }
        }

        Bitboard attacks = 0;
        switch (type) {
            case PAWN: {
                int direction = (side == WHITE) ? 8 : -8;
                int start_rank = (side == WHITE) ? 8 : 48;

                // Forward move
                int forward = sq + direction;
                if (forward >= 0 && forward < 64 && !get_bit(board->occupancy, forward)) {
                    list->moves[list->count++] = (Move){sq, forward, 0};

                    // Double push
                    if ((side == WHITE && sq >= 8 && sq <= 15) || (side == BLACK && sq >= 48 && sq <= 55)) {
                        int double_forward = forward + direction;
                        if (double_forward >= 0 && double_forward < 64 && !get_bit(board->occupancy, double_forward)) {
                            list->moves[list->count++] = (Move){sq, double_forward, 0};
                        }
                    }
                }

                // Captures
                int cap_offsets[] = {-direction - 1, -direction + 1};
                for (int i = 0; i < 2; i++) {
                    int target = sq + cap_offsets[i];
                    if (target >= 0 && target < 64) {
                        int sq_file = sq % 8;
                        int target_file = target % 8;
                        // Ensure the pawn is moving diagonally by exactly one file
                        if (abs(sq_file - target_file) == 1) {
                            if (get_bit(board->occupancy, target)) {
                                // Ensure it's an enemy piece
                                Color target_color = get_bit(board->color_pieces[WHITE], target) ? WHITE : BLACK;
                                if (target_color != side) {
                                    list->moves[list->count++] = (Move){sq, target, 0};
                                }
                            }
                        }
                    }
                }
                break;
            }
            case KNIGHT: attacks = get_knight_attacks(sq); break;
            case BISHOP: attacks = get_bishop_attacks(sq, board->occupancy); break;
            case ROOK: attacks = get_rook_attacks(sq, board->occupancy); break;
            case QUEEN: attacks = get_rook_attacks(sq, board->occupancy) | get_bishop_attacks(sq, board->occupancy); break;
            case KING: attacks = get_king_attacks(sq); break;
            default: break;
        }

        if (type != PAWN) {
            while (attacks) {
                int target = __builtin_ctzll(attacks);
                // Only add move if target is empty or contains an enemy piece
                if (!get_bit(board->color_pieces[side], target)) {
                    list->moves[list->count++] = (Move){sq, target, 0};
                }
                attacks &= ~(1ULL << target);
            }
        }
    }
}

/* To be implemented: is_square_attacked, generate_legal_moves */
bool is_square_attacked(Board *board, int sq, Color attacker_color) {
    // King attacks
    for (int i = 0; i < 12; i++) {
        if (i == (attacker_color == WHITE ? 5 : 11)) { // King
            Bitboard king_pos = board->pieces[i];
            while (king_pos) {
                int k_sq = __builtin_ctzll(king_pos);
                if (get_bit(get_king_attacks(k_sq), sq)) return true;
                king_pos &= ~(1ULL << k_sq);
            }
        } else if (i >= (attacker_color == WHITE ? 0 : 6) && i < (attacker_color == WHITE ? 6 : 12)) {
            // Other pieces of attacker_color
            if (i == (attacker_color == WHITE ? 1 : 7)) { // Knight
                Bitboard knight_pos = board->pieces[i];
                while (knight_pos) {
                    int n_sq = __builtin_ctzll(knight_pos);
                    if (get_bit(get_knight_attacks(n_sq), sq)) return true;
                    knight_pos &= ~(1ULL << n_sq);
                }
            }
            // Sliders
            if (i == (attacker_color == WHITE ? 3 : 9) || i == (attacker_color == WHITE ? 4 : 10) || i == (attacker_color == WHITE ? 2 : 8)) {
                // Rook, Queen, Bishop
                // For efficiency, we check from the target square instead of iterating over all pieces
                // But for now, a simple check:
                Bitboard pieces = board->pieces[i];
                while (pieces) {
                    int p_sq = __builtin_ctzll(pieces);
                    Bitboard attacks = 0;
                    if (i == (attacker_color == WHITE ? 3 : 9)) attacks = get_rook_attacks(p_sq, board->occupancy);
                    else if (i == (attacker_color == WHITE ? 2 : 8)) attacks = get_bishop_attacks(p_sq, board->occupancy);
                    else attacks = get_rook_attacks(p_sq, board->occupancy) | get_bishop_attacks(p_sq, board->occupancy);

                    if (get_bit(attacks, sq)) return true;
                    pieces &= ~(1ULL << p_sq);
                }
            }
        }
    }
    // Pawns are special
    Color side = attacker_color;
    Bitboard pawns = board->color_pieces[side == WHITE ? WHITE : BLACK] & (board->pieces[WHITE_PAWN] | board->pieces[BLACK_PAWN]);
    // Wait, the above logic is wrong. Let's fix:
    Bitboard attacker_pawns = (side == WHITE) ? board->pieces[WHITE_PAWN] : board->pieces[BLACK_PAWN];
    int pawn_dir = (side == WHITE) ? -8 : 8; // Attacked by pawn from above/below
    Bitboard pawn_attacks = 0;
    // Pawn at sq - direction +/- 1 attacks sq
    // So we check if pawns are at (sq + direction - 1) and (sq + direction + 1)
    int p_sq1 = sq + pawn_dir - 1;
    int p_sq2 = sq + pawn_dir + 1;
    if (p_sq1 >= 0 && p_sq1 < 64 && (sq % 8 != 0) && get_bit(attacker_pawns, p_sq1)) return true;
    if (p_sq2 >= 0 && p_sq2 < 64 && (sq % 8 != 7) && get_bit(attacker_pawns, p_sq2)) return true;

    return false;
}

void generate_legal_moves(Board *board, MoveList *list) {
    generate_pseudo_legal_moves(board, list);
    int legal_count = 0;

    for (int i = 0; i < list->count; i++) {
        UndoInfo undo;
        make_move(board, list->moves[i], &undo);

        // Find new king position
        Color side = (board->side_to_move == WHITE) ? BLACK : WHITE;
        Bitboard king_pos = board->pieces[side == WHITE ? WHITE_KING : BLACK_KING];
        int king_sq = __builtin_ctzll(king_pos);

        if (!is_square_attacked(board, king_sq, (side == WHITE ? BLACK : WHITE))) {
            list->moves[legal_count++] = list->moves[i];
        }

        unmake_move(board, undo);
    }
    list->count = legal_count;
}
