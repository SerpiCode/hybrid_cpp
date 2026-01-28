# main.py

import sys
import os
import warnings
import time
import pandas
import random

from pathlib import Path
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score, davies_bouldin_score, calinski_harabasz_score

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
warnings.simplefilter("ignore")

from decoder import decode

# Configuração da métrica
metric = 'sil'
random.seed(int(round(time.time() * 1000)))

# Arquivo de saída
output = open(f"results/results/{metric}-results.csv", "wt", encoding="utf-8")
output.write("dataset,time,number-clusters,silhouette,davies-bouldin,calinski-harabasz\n")
output.flush()

print("Clustering data lakes (via results.txt)...")

# Lê todos os cromossomos do results.txt de uma vez
results_path = './results/results.txt'
if not os.path.exists(results_path):
    print(f"results.txt not found. Aborting.")
    exit(1)

with open(results_path, "r", encoding="utf-8") as f:
    results_lines = [line.strip() for line in f if line.strip()]

# Percorre os datasets
file_path = Path("./data-lakes/").rglob("*.csv")
files = sorted([str(f) for f in file_path])  # ordenação importante se os arquivos não estiverem na ordem correta

if len(results_lines) < len(files):
    print(f"Erro: há mais datasets ({len(files)}) do que linhas no results.txt ({len(results_lines)})")
    exit(1)

for idx, file in enumerate(files):
    # print(f"Working on dataset {file}")

    dataset = pandas.read_csv(file)
    dataset = dataset.iloc[:, :-1].values  # Remove a coluna da classe
    dataset = StandardScaler().fit_transform(dataset)

    # Pega a linha correspondente do results.txt
    # Extrai apenas o nome do arquivo atual (sem caminho)
    file_name = os.path.basename(file)

    # Procura no results.txt a linha que corresponde a esse dataset
    matching_line = None
    for line in results_lines:
        dataset_name = os.path.basename(line.split()[0])
        dataset_name = dataset_name[:-1] 
        if dataset_name == file_name:
            matching_line = line
            break

    if matching_line is None:
        print(f"Nenhuma linha correspondente a {file_name} em results.txt.")
        continue

    parts = matching_line.strip().split()

    if len(parts) < 3:
        print(f"Line {idx+1} do results.txt é inválida.")
        continue

    # Extrai cromossomo e tempo
    chromosome_str = parts[1:-1]  # Remove o nome do dataset e o tempo
    time_taken = parts[-1]

    # Converte cromossomo para lista de floats, com tratamento de erro
    chromosome = []
    for gene in chromosome_str:
        gene = gene.strip().replace(",", "")
        if gene == "":
            continue
        try:
            chromosome.append(float(gene))
        except ValueError:
            print(f"Skipping invalid gene '{gene}' in line {idx+1}: {line}")
            chromosome = []
            break

    if not chromosome:
        continue  # pula o dataset se cromossomo inválido

    try:
        print(f'Dataset do results.txt: {parts[0]}\nDataset atual: {file}\n')
        # Decodifica e roda o KMeans
        k, A = decode(dataset, chromosome)
        X_sel = dataset[:, [i for i in range(len(A)) if A[i] == 1]]

        if X_sel.shape[1] == 0 or k <= 1:
            print(f"Dataset {file} com seleção inválida: {X_sel.shape[1]} features, {k} clusters.")
            continue

        labels = KMeans(n_clusters=k, n_init=10, random_state=0).fit_predict(X_sel)

        sil = silhouette_score(X_sel, labels)
        db = davies_bouldin_score(X_sel, labels)
        ch = calinski_harabasz_score(X_sel, labels)

        time_taken = time_taken[:-1]

        output.write(f"{file},{time_taken},{k},{sil},{db},{ch}\n")
        output.flush()

        # print(f"✓ {file} processado com sucesso.")

    except Exception as e:
        print(f"Erro ao processar {file}: {e}")
        continue

output.close()
print("Finished!")
