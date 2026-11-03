import subprocess
import sys
import os
import re

def get_best_move(fen, weights_file="model_0.bin"):
    """
    Calls the C chess engine to get the best move for a given FEN.
    Includes timeout and validation as per Spec 5.I.
    """
    # Locate binary relative to script directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    binary_name = os.environ.get("CHESS_ENGINE_BIN", "chess_engine")
    binary_path = os.path.join(script_dir, binary_name)

    try:
        # Use timeout to ensure we stay under 10s (spec: slightly less than limit)
        result = subprocess.run(
            [binary_path, fen, weights_file],
            capture_output=True,
            text=True,
            timeout=9.0,
            check=True
        )
        move = result.stdout.strip()

        # Validate UCI format: ^[a-h][1-8][a-h][1-8][qrbn]?$
        if not re.match(r"^[a-h][1-8][a-h][1-8][qrbn]?$", move):
            raise ValueError(f"Invalid UCI move returned: {move}")

        return move

    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, ValueError) as e:
        # Contingency: if main move fails, fetch a legal move from --list-moves
        return get_contingency_move(binary_path, fen)

def get_contingency_move(binary_path, fen):
    """
    Fetches list of legal moves and returns the first one.
    """
    try:
        result = subprocess.run(
            [binary_path, "--list-moves", fen],
            capture_output=True,
            text=True,
            timeout=2.0,
            check=True
        )
        moves = result.stdout.strip().split('\n')
        if moves and moves[0]:
            return moves[0]
    except Exception:
        pass

    # Absolute last resort (should never happen with legal FEN)
    return "e2e4"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python agent_wrapper.py <FEN> [weights_file]")
        sys.exit(1)

    input_fen = sys.argv[1]
    weights = sys.argv[2] if len(sys.argv) > 2 else "model_0.bin"

    move = get_best_move(input_fen, weights)
    if move:
        print(move)
    else:
        sys.exit(1)
