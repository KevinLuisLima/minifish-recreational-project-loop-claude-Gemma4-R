#ifndef EVALUATION_H
#define EVALUATION_H

#include "types.h"
#include "position.h"

typedef struct EvaluationEngine {
    const char* model_name;
    float* weights;
    uint8_t origin; // 0 = dataset, 1 = synthetic
    uint16_t weight_count;

    int (*evaluate)(struct EvaluationEngine* self, Board* board);
    int (*load_weights)(struct EvaluationEngine* self, const char* filename);
} EvaluationEngine;

EvaluationEngine* create_basic_engine();
EvaluationEngine* create_hf_engine();
void destroy_engine(EvaluationEngine* engine);

#endif
