#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "types.h"
#include "position.h"
#include "evaluation.h"

typedef struct {
    char model_name[32];
    int wins;
    int draws;
    int losses;
    int moves_played;
    int suboptimal_moves;
    int total_legal_games;
    uint8_t origin; // 0 = dataset, 1 = synthetic
} ModelStats;

typedef struct {
    int threshold;
    int judge_depth;
    const char* judge_type;
} JudgeConfig;

void run_tournament(int num_models, const char** weight_files, JudgeConfig config);

#endif
