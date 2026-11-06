import struct
import random
import os

# Constants matching the C structure (EvalWeights)
# material[6], pst[6][64], mobility_weight, pawn_structure_weight
# Material: 6 floats
# PST: 6 * 64 floats
# Weights: 2 floats
# Total: 6 + 384 + 2 = 392 floats
# Format: < (little-endian) and 392 'f' (float32)

def create_default_weights(filename="weights.bin"):
    # Default material values (centipawns)
    material = [
        100.0, # Pawn
        320.0, # Knight
        330.0, # Bishop
        500.0, # Rook
        900.0, # Queen
        0.0    # King (not counted in material)
    ]

    # Simple PSTs: Just zeros or basic values for now
    # In a real scenario, these would be derived from the dataset
    pst = []
    for p in range(6):
        pst.extend([0.0] * 64)

    mobility_weight = 10.0
    pawn_structure_weight = 20.0

    # Pack data
    data = struct.pack('<' + 'f' * 6, *material)
    data += struct.pack('<' + 'f' * 384, *pst)
    data += struct.pack('<ff', mobility_weight, pawn_structure_weight)

    with open(filename, "wb") as f:
        f.write(data)
    print(f"Default weights saved to {filename}")

if __name__ == "__main__":
    create_default_weights()
