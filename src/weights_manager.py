import os
import struct
import random

# Configurações do Dataset
DATASET_REPO = "linrock/bullet-training-data"
DATASET_FILE = "training_data.bin"

def download_hf_data():
    """Consome a API do HuggingFace para baixar os dados de treino."""
    print(f"Consumindo API do HuggingFace: {DATASET_REPO}...")
    # Como não temos a lib huggingface_hub instalada, simulamos o download
    # Para um sistema real, usaríamos: requests.get(url)
    print("Atenção: Usando pesos sintéticos para simulação de API.")
    return None

def generate_model_weights(base_weights, variation_factor=0.1, model_id=0):
    """
    Modifica levemente os pesos das variáveis para gerar novos modelos.
    Usa apenas a biblioteca padrão 'random' para evitar dependências.
    """
    modified_weights = []
    for w in base_weights:
        noise = random.uniform(-variation_factor, variation_factor)
        modified_weights.append(w + noise)

    filename = f"model_{model_id}.bin"
    with open(filename, "wb") as f:
        # 'f' para float (4 bytes)
        f.write(struct.pack('f' * len(modified_weights), *modified_weights))

    print(f"Modelo {model_id} gerado: {filename}")
    return filename

def main():
    hf_path = download_hf_data()

    # Pesos base: P, N, B, R, Q, K
    base_weights = [100.0, 320.0, 330.0, 500.0, 900.0, 20000.0]

    models = []
    for i in range(3):
        model_file = generate_model_weights(base_weights, model_id=i)
        models.append(model_file)

    print("\nModelos prontos para o torneio em C.")
    print(f"Arquivos: {models}")

if __name__ == "__main__":
    main()
