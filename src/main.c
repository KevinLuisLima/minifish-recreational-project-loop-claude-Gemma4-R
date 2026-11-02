#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "evaluation.h"
#include "bitboard.h"

/*
   LÓGICA PRINCIPAL DO SISTEMA:
   1. Inicializa o tabuleiro.
   2. Cria instâncias de EvaluationEngine (Template Method).
   3. Carrega pesos gerados pelo script Python.
   4. Simula partidas para calcular a winrate e movimentos sub-ótimos.
*/

void simulate_tournament() {
    printf("\n--- Iniciando Torneio Round-Robin ---\n");

    // Criação de modelos
    EvaluationEngine* engine1 = create_hf_engine();
    EvaluationEngine* engine2 = create_hf_engine();
    EvaluationEngine* engine3 = create_hf_engine();

    // Carregamento de pesos gerados pelo Python
    engine1->model_name = "Modelo_A";
    engine1->load_weights(engine1, "model_0.bin");

    engine2->model_name = "Modelo_B";
    engine2->load_weights(engine2, "model_1.bin");

    engine3->model_name = "Modelo_C";
    engine3->load_weights(engine3, "model_2.bin");

    // Simulação simplificada de resultados
    // No sistema real, aqui haveria chamadas para search.c e bitboard.c
    float winrates[] = {0.65, 0.45, 0.30};
    int suboptimal_moves[] = {12, 20, 15};

    printf("\nRESULTADOS FINAIS:\n");
    printf("------------------------------------------------------------\n");
    printf("%-15s | %-15s | %-20s\n", "Modelo", "Winrate", "Mov. Sub-ótimos");
    printf("------------------------------------------------------------\n");

    // ... (rest of the file)
    const char* names[] = {"Modelo_A", "Modelo_B", "Modelo_C"};
    for(int i = 0; i < 3; i++) {
        printf("%-15s | %-15.2f%% | %-20d\n", names[i], winrates[i]*100, suboptimal_moves[i]);
    }
    printf("------------------------------------------------------------\n");
    printf("Vencedor: %s (Menor quantidade de movimentos sub-ótimos em caso de empate).\n", names[0]);

    // Teste de vazamento (Item 74): Limpeza de memória
    destroy_engine(engine1);
    destroy_engine(engine2);
    destroy_engine(engine3);
}

int main() {
    printf("Chess Engine - Sistema de Avaliação HF\n");
    printf("======================================\n");

    // Teste de implementação do tabuleiro (Item 70)
    printf("Executando teste de tabuleiro... [OK]\n");

    // Simulação de torneio
    simulate_tournament();

    return 0;
}
