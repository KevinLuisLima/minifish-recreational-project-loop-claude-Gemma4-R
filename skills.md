# Chess AI Project - Documentation

## 1. Project Overview
This project implements a resource-efficient Chess AI engine written in C, designed for the FIDE & Google Efficient Chess AI Challenge. The goal was to create an agent capable of playing legal chess under strict resource constraints.

### Goals
- Functional chess engine in C.
- Kaggle-compatible interface (FEN $\rightarrow$ UCI move).
- Weight-based evaluation with an explainable tuning pipeline.
- Resource efficiency ($\le 64$ KB binary, $\le 5$ MB RAM).

## 2. Technical Implementation

### Architecture
The project follows a modular design:
- **Representation:** Bitboards (64-bit integers) for all piece types and occupancy.
- **Move Generation:** 
  - Pre-calculated attack tables for Knights and Kings.
  - Iterative sliding attacks for Rooks and Bishops.
  - Legality filter using a "make-move $\rightarrow$ check-king $\rightarrow$ unmake-move" loop.
- **Search:**
  - Iterative Deepening (implemented as fixed depth in this version).
  - Alpha-Beta Pruning.
  - Quiescence Search to mitigate the horizon effect.
  - Transposition Table (TT) using Zobrist Hashing for state caching.
- **Evaluation:**
  - Material value + Piece-Square Tables (PST).
  - Evaluation is relative to the side to move.
  - Weights are loaded from an external binary file.

### Design Patterns
- **Behavioral Pattern:** The project uses a structured search pipeline (Search $\rightarrow$ Quiescence $\rightarrow$ Evaluation) following the logic of the Template Method.

## 3. Verification Report

| Affirmation | Command | Result | Date |
| :--- | :--- | :--- | :--- |
| Compiles without warnings | `make` | Success | 2026-11-06 |
| Correct legal move generation | `./bin/chess_engine <start_fen> <weights>` | 20 moves (Correct) | 2026-11-06 |
| Binary size within limits | `ls -lh bin/chess_engine` | $\le 64$ KB | 2026-11-06 |
| Weight loading works | `./bin/chess_engine <fen> weights.bin` | Success | 2026-11-06 |
| TT reduces node count | `./bin/chess_engine` | $\sim 4$x speedup | 2026-11-06 |
| Wrapper returns UCI move | `python3 kaggle_wrapper.py` | "b1a3" (Correct) | 2026-11-06 |

## 4. Learning & Development

### Iterations & Challenges
- **Move Generation:** Initially, sliding pieces were capturing their own pieces. This was fixed by adding an occupancy check against the moving player's bitboard.
- **Zobrist Hashing:** Implementing a consistent hash update during `make_move` and `unmake_move` was critical for the Transposition Table to function correctly.
- **Dataset Access:** The HuggingFace API endpoints changed; the pipeline was adapted to handle fallback synthetic weights to ensure the tournament and evaluation pipeline remained functional.

### Pending Improvements
- Implement full Iterative Deepening with a wall-clock timer.
- Add more complex evaluation terms (mobility, king safety).
- Implement a full game simulator in C to run more accurate local tournaments.

## 5. Resource Measurement
- **Binary Size:** Verified under 64KB.
- **Memory Usage:** The Transposition Table is fixed at $2^{16}$ entries, keeping RAM usage well under 5MB.
- **Time per move:** Average search at depth 3 completes in milliseconds, far below the 10s limit.
