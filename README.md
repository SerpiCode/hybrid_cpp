# BRKGA for Clustering 

Este repositório é destinado a implementação do algoritmo BRKGA para solucionar problemas de agrupamento no contexto de data lakes.

## How to Run

### Código principal (algoritmo)

```console
g++ -std=c++20 -fopenmp -O3 -Ibrkga_mp_ipr_cpp/brkga_mp_ipr source-code/main.cpp source-code/decoder.cpp source-code/instance.cpp source-code/silhouette.cpp source-code/davies_bouldin.cpp -o main

./main seed config_file max_time

example:
./main 42 config.conf 300
```

---

### irace (otimização de hiperparâmetros)

```console
g++ -std=c++20 -fopenmp -O3 -Ibrkga_mp_ipr_cpp/brkga_mp_ipr irace/main.cpp source-code/decoder.cpp source-code/instance.cpp source-code/silhouette.cpp source-code/davies_bouldin.cpp -o main-irace

./main-irace
```

> Nota: o irace otimiza **apenas** os parâmetros informados no `parameters.txt`, os demais estão hard-coded no arquivo `irace/main.cpp`.

## Resultados

Os resultados obtidos podem ser visualizados neste [link](https://drive.google.com/drive/folders/1MqMy6_wWTJ0vqAlas9AHIEZFipAlwLPv?usp=sharing).
