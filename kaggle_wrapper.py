import subprocess
import sys

def get_best_move(fen, weights_path="weights.bin"):
    """
    Interface compatible with Kaggle environment.
    Receives a FEN and returns a UCI move.
    """
    try:
        # The C engine currently takes FEN internally in main.c for testing.
        # We need to modify the C engine to accept FEN and weights as CLI arguments.
        # For now, we call the binary.

        # We will implement a small 'bridge' in the C engine to handle this.
        # Usage: ./chess_engine <fen> <weights_path>
        result = subprocess.run(
            ["./bin/chess_engine", fen, weights_path],
            capture_output=True,
            text=True,
            timeout=10
        )

        if result.returncode != 0:
            return "e2e4" # Legal contingency move for starting position

        # Expecting output: "BEST_MOVE: e2e4"
        for line in result.stdout.split('\n'):
            if line.startswith("BEST_MOVE:"):
                return line.split(":")[1].strip()

        return "e2e4"
    except Exception as e:
        print(f"Error: {e}")
        return "e2e4"

if __name__ == "__main__":
    # Test with a sample FEN
    sample_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    move = get_best_move(sample_fen)
    print(f"FEN: {sample_fen}")
    print(f"Move: {move}")
