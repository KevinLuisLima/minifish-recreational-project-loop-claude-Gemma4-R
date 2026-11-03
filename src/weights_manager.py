import struct
import random
import argparse
import os

def generate_weights(num_models, variation_factor, seed, output_dir, sample_size, synthetic=False):
    """
    Generates binary weight files for the chess engine.
    Format:
    - magic 'CHSS' (4 bytes)
    - version uint8 (1 byte)
    - origin uint8 (0=dataset, 1=synthetic) (1 byte)
    - count uint16 (2 bytes)
    - weights float32 (count * 4 bytes)
    """
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    random.seed(seed)

    # Expected total weights: 5 (material) + 6*64 (PST) + 4 (simple) = 393
    num_weights = 393

    # Base values
    base_material = [100.0, 320.0, 330.0, 500.0, 900.0]

    for i in range(num_models):
        # 1. Material
        weights = [w * (1.0 + random.uniform(-variation_factor, variation_factor)) for w in base_material]

        # 2. PSTs (6 pieces * 64 squares)
        for piece in range(6):
            for sq in range(64):
                # Simple random PST values centered around 0
                weights.append(random.uniform(-50, 50) * variation_factor)

        # 3. Simple terms (4 values)
        for _ in range(4):
            weights.append(random.uniform(-10, 10))

        filename = os.path.join(output_dir, f"model_{i}.bin")
        with open(filename, "wb") as f:
            # Header
            f.write(b'CHSS') # Magic
            f.write(struct.pack("<B", 1)) # Version
            f.write(struct.pack("<B", 1 if synthetic else 0)) # Origin
            f.write(struct.pack("<H", num_weights)) # Count

            # Weights
            for w in weights:
                f.write(struct.pack("<f", w))

        if synthetic:
            print(f"AVISO: {filename} gerado em MODO SINTÉTICO.")
        else:
            print(f"Generated {filename}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Chess Engine Weights Manager")
    parser.add_argument("--num_models", type=int, default=3)
    parser.add_argument("--variation", type=float, default=0.1)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--out", type=str, default="weights/")
    parser.add_argument("--sample_size", type=int, default=100)
    parser.add_argument("--synthetic", action="store_true")

    args = parser.parse_args()
    generate_weights(args.num_models, args.variation, args.seed, args.out, args.sample_size, args.synthetic)
