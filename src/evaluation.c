#include "evaluation.h"
#include "bitboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   LÓGICA PRINCIPAL:
   O sistema utiliza a estrutura EvaluationEngine para desacoplar a
   estratégia de avaliação do motor de busca.
   1. O motor de busca chama 'engine->evaluate()'.
   2. A implementação concreta (Basic ou HF) aplica os pesos aos materiais/posições.
*/

// Implementação de avaliação básica (Heurística simples)
int basic_evaluate(EvaluationEngine* self, Board* board) {
    int score = 0;
    // Valores simples de peças: P=100, N=320, B=330, R=500, Q=900, K=20000
    int piece_values[] = {100, 320, 330, 500, 900, 20000};

    for (int type = 0; type < 6; type++) {
        score += count_bits(board->pieces[WHITE][type]) * piece_values[type];
        score -= count_bits(board->pieces[BLACK][type]) * piece_values[type];
    }
    return score;
}

void basic_load_weights(EvaluationEngine* self, const char* filename) {
    printf("Basic Engine: Usando pesos internos padrão.\n");
}

EvaluationEngine* create_basic_engine() {
    EvaluationEngine* engine = malloc(sizeof(EvaluationEngine));
    engine->model_name = "Basic-Heuristic";
    engine->evaluate = basic_evaluate;
    engine->load_weights = basic_load_weights;
    engine->weights = NULL;
    return engine;
}

// Implementação de avaliação baseada em HuggingFace
int hf_evaluate(EvaluationEngine* self, Board* board) {
    if (!self->weights) return 0;

    int score = 0;
    // Aqui a lógica usaria os pesos carregados do HF para cada tipo de peça
    // e possivelmente a posição (simplificado para o esqueleto)
    for (int type = 0; type < 6; type++) {
        score += count_bits(board->pieces[WHITE][type]) * (int)self->weights[type];
        score -= count_bits(board->pieces[BLACK][type]) * (int)self->weights[type];
    }
    return score;
}

void hf_load_weights(EvaluationEngine* self, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Erro ao abrir pesos: %s\n", filename);
        return;
    }
    self->weights = malloc(6 * sizeof(float));
    fread(self->weights, sizeof(float), 6, f);
    fclose(f);
    printf("HF Engine: Pesos carregados de %s\n", filename);
}

// ... (rest of the file)
EvaluationEngine* create_hf_engine() {
    EvaluationEngine* engine = malloc(sizeof(EvaluationEngine));
    engine->model_name = "HF-Weight-Model";
    engine->evaluate = hf_evaluate;
    engine->load_weights = hf_load_weights;
    engine->weights = NULL;
    return engine;
}

void destroy_engine(EvaluationEngine* engine) {
    if (engine) {
        if (engine->weights) {
            free(engine->weights);
        }
        free(engine);
    }
}
