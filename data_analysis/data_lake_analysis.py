import os
import pandas as pd
import numpy as np

# Caminho base
BASE_DIR = "../data-lakes/clustering"

# Lista para armazenar as estatísticas
resumo = []

# Percorre todas as subpastas
for pasta in sorted(os.listdir(BASE_DIR)):
    caminho_pasta = os.path.join(BASE_DIR, pasta)
    if not os.path.isdir(caminho_pasta):
        continue

    n_linhas = []
    n_colunas = []
    arquivos_csv = [f for f in os.listdir(caminho_pasta) if f.endswith(".csv")]

    # Percorre os datasets dentro da pasta
    for arquivo in arquivos_csv:
        caminho_arquivo = os.path.join(caminho_pasta, arquivo)
        try:
            df = pd.read_csv(caminho_arquivo)
            n_linhas.append(df.shape[0])
            n_colunas.append(df.shape[1])
        except Exception as e:
            print(f"⚠️ Erro ao ler {caminho_arquivo}: {e}")

    if len(n_linhas) == 0:
        continue

    # Calcula estatísticas
    resumo.append({
        "datalake": pasta,
        "num_datasets": len(n_linhas),
        "min_objetos": int(np.min(n_linhas)),
        "media_objetos": float(np.mean(n_linhas)),
        "max_objetos": int(np.max(n_linhas)),
        "min_variaveis": int(np.min(n_colunas)),
        "media_variaveis": float(np.mean(n_colunas)),
        "max_variaveis": int(np.max(n_colunas))
    })

# Cria DataFrame final
df_resumo = pd.DataFrame(resumo)

# Salva como CSV
df_resumo.to_csv("resumo_datalakes.csv", index=False)

print("✅ Arquivo 'resumo_datalakes.csv' gerado com sucesso!")
