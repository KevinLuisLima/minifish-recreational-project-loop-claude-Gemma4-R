#ifndef EVALUATION_H
#define EVALUATION_H

#include "types.h"

// Estrutura para o Padrão Template Method
// Define a "interface" de avaliação que cada modelo deve implementar
typedef struct EvaluationEngine {
    const char* model_name;
    float* weights;

    // Ponteiro de função para a lógica de avaliação (O "Template Method")
    int (*evaluate)(struct EvaluationEngine* self, Board* board);

    // Método para carregar pesos do arquivo
    void (*load_weights)(struct EvaluationEngine* self, const char* filename);
} EvaluationEngine;

// Implementações concretas de avaliação
EvaluationEngine* create_basic_engine();
EvaluationEngine* create_hf_engine();
void destroy_engine(EvaluationEngine* engine);

#endif
