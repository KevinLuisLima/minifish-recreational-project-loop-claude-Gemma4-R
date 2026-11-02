# Documentação do Projeto: Chess Engine HF

## 1. Visão Geral
Este projeto implementa um motor de xadrez em C focado na experimentação de pesos de avaliação provenientes de datasets do HuggingFace. O sistema permite a geração de múltiplos modelos com variações leves nos pesos para testar a eficiência de cada variação através de um torneio round-robin.

## 2. Arquitetura Técnica

### Padrão de Design: Template Method
Para permitir a flexibilidade na troca de modelos de avaliação sem alterar a lógica de busca, foi implementada a estrutura `EvaluationEngine` em `evaluation.h`:

```c
typedef struct EvaluationEngine {
    const char* model_name;
    float* weights;
    int (*evaluate)(struct EvaluationEngine* self, Board* board);
    void (*load_weights)(struct EvaluationEngine* self, const char* filename);
} EvaluationEngine;
```

**Como funciona:**
- O motor de busca chama a função `evaluate` através de um ponteiro.
- Diferentes implementações (ex: `create_basic_engine` vs `create_hf_engine`) definem comportamentos distintos para a mesma "interface", permitindo que novos modelos sejam adicionados apenas criando novas funções de avaliação.

### Componentes do Sistema
- **Bitboard (`bitboard.c/h`)**: Representação do tabuleiro usando inteiros de 64 bits para alta performance.
- **Evaluation (`evaluation.c/h`)**: Lógica de pontuação baseada em material e pesos externos.
- **Weights Manager (`weights_manager.py`)**: Script Python que consome a API do HuggingFace e gera variações de pesos binários.
- **Tournament (`main.c`)**: Orquestrador que carrega os modelos e calcula métricas de vitória e eficiência.

## 3. Lições Aprendidas e Iterações

### Tentativas e Ajustes
1. **Dependências Python**: Inicialmente tentei usar `numpy` para a manipulação de pesos. Identifiquei que isso criava uma barreira de instalação. Iterei a solução para usar as bibliotecas `random` e `struct` do Python padrão, tornando o sistema portátil.
2. **Visibilidade em C**: Houve um erro inicial de "implicit declaration" para a função `count_bits`. A correção envolveu a reorganização dos headers para garantir que `evaluation.c` tivesse visibilidade total de `bitboard.h`.
3. **Consumo de API**: Devido a limites de ambiente, implementei um fallback de pesos sintéticos que simula o comportamento da API do HuggingFace, garantindo que o loop de desenvolvimento não fosse interrompido por falhas de rede.

### O que deu certo
- O uso de **ponteiros de função** em C provou ser a maneira mais eficiente de implementar o padrão Template Method, mantendo a performance de linguagem de baixo nível.
- A separação entre a geração de pesos (Python) e a execução (C) permitiu ciclos de iteração rápidos.

### Melhorias Futuras
- Implementação de algoritmos de busca mais avançados como *Alpha-Beta Pruning* e *Iterative Deepening*.
- Expansão do `weights_manager.py` para ler a estrutura real de tensores do HuggingFace.
- Implementação de tabelas de finalmente (Endgame Tablebases).

## 4. Métricas de Sucesso
- **Winrate**: Calculada via torneio round-robin.
- **Eficiência**: Medida pela quantidade de movimentos sub-ótimos por ação.
- **Critério de Desempate**: O modelo com menor número de movimentos sub-ótimos é declarado vencedor.
