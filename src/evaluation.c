#include "evaluation.h"
#include "bitboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Default Piece-Square Tables for the basic engine
static const int DEFAULT_PAWN_PST[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    0,  5, 10, 15, 15, 10,  5,  0,
    0,  0,  0,  5,  5,  0,  0,  0,
    0,  0,  5, 10, 10,  5,  0,  0,
    5, 10, 10, 20, 20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

int basic_evaluate(EvaluationEngine* self, Board* board) {
    int score = 0;
    int piece_values[] = {100, 320, 330, 500, 900, 20000};

    for (int type = 0; type < 6; type++) {
        int white_count = count_bits(board->pieces[WHITE][type]);
        int black_count = count_bits(board->pieces[BLACK][type]);
        score += (white_count - black_count) * piece_values[type];
    }

    for (int i = 0; i < 64; i++) {
        if (board->pieces[WHITE][PAWN] & (1ULL << i)) score += DEFAULT_PAWN_PST[i];
        if (board->pieces[BLACK][PAWN] & (1ULL << i)) score -= DEFAULT_PAWN_PST[63 - i];
    }

    return (board->sideToMove == WHITE) ? score : -score;
}

int basic_load_weights(EvaluationEngine* self, const char* filename) {
    (void)self;
    (void)filename;
    return 0;
}

EvaluationEngine* create_basic_engine() {
    EvaluationEngine* engine = malloc(sizeof(EvaluationEngine));
    if (!engine) return NULL;
    engine->model_name = "Basic-Heuristic";
    engine->evaluate = basic_evaluate;
    engine->load_weights = basic_load_weights;
    engine->weights = NULL;
    engine->origin = 0;
    engine->weight_count = 0;
    return engine;
}

int hf_evaluate(EvaluationEngine* self, Board* board) {
    if (!self->weights) return 0;

    int score = 0;
    // 1. Material (first 5 weights)
    for (int type = 0; type < 5; type++) {
        int white_count = count_bits(board->pieces[WHITE][type]);
        int black_count = count_bits(board->pieces[BLACK][type]);
        score += (int)((white_count - black_count) * self->weights[type]);
    }

    // 2. Piece-Square Tables (next 6 * 64 weights)
    // Format: [PAWN_PST, KNIGHT_PST, BISHOP_PST, ROOK_PST, QUEEN_PST, KING_PST]
    for (int type = 0; type < 6; type++) {
        float* pst = &self->weights[5 + (type * 64)];
        for (int sq = 0; sq < 64; sq++) {
            if (board->pieces[WHITE][type] & (1ULL << sq)) score += (int)pst[sq];
            if (board->pieces[BLACK][type] & (1ULL << sq)) score -= (int)pst[63 - sq];
        }
    }

    // 3. Simple terms (mobility, safety, etc.)
    // Offset = 5 + (6 * 64) = 389.
    // For this version, we'll just use the remaining 4 weights as constants.
    // mobility = self->weights[389] * (total_legal_moves);
    // score += (board->sideToMove == WHITE ? mobility : -mobility);

    return (board->sideToMove == WHITE) ? score : -score;
}

int hf_load_weights(EvaluationEngine* self, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return -1;

    char magic[4];
    if (fread(magic, 1, 4, f) != 4 || memcmp(magic, "CHSS", 4) != 0) {
        fclose(f);
        return -2;
    }

    uint8_t version;
    if (fread(&version, 1, 1, f) != 1 || version != 1) {
        fclose(f);
        return -3;
    }

    uint8_t origin;
    if (fread(&origin, 1, 1, f) != 1) {
        fclose(f);
        return -4;
    }

    uint16_t count;
    if (fread(&count, 2, 1, f) != 1) {
        fclose(f);
        return -5;
    }

    // The expected count is 393 (5 mat + 6*64 pst + 4 simple)
    if (count != 393) {
        fclose(f);
        return -6;
    }

    float* new_weights = malloc(count * sizeof(float));
    if (!new_weights) {
        fclose(f);
        return -7;
    }

    if (fread(new_weights, sizeof(float), count, f) != count) {
        free(new_weights);
        fclose(f);
        return -8;
    }

    // Validate NaN/Inf
    for (int i = 0; i < count; i++) {
        if (isnan(new_weights[i]) || isinf(new_weights[i])) {
            free(new_weights);
            fclose(f);
            return -9;
        }
    }

    fclose(f);

    if (self->weights) free(self->weights);
    self->weights = new_weights;
    self->origin = origin;
    self->weight_count = count;

    return 0;
}

EvaluationEngine* create_hf_engine() {
    EvaluationEngine* engine = malloc(sizeof(EvaluationEngine));
    if (!engine) return NULL;
    engine->model_name = "HF-Weight-Model";
    engine->evaluate = hf_evaluate;
    engine->load_weights = hf_load_weights;
    engine->weights = NULL;
    engine->origin = 0;
    engine->weight_count = 0;
    return engine;
}

void destroy_engine(EvaluationEngine* engine) {
    if (engine) {
        if (engine->weights) free(engine->weights);
        free(engine);
    }
}
