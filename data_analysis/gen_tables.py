import pandas as pd

# Caminho do CSV gerado anteriormente
csv_path = "resumo_datalakes.csv"

# Lê o CSV
df = pd.read_csv(csv_path)

# Garante a ordem das colunas (caso o CSV tenha sido modificado)
colunas = [
    "datalake", "num_datasets",
    "min_objetos", "media_objetos", "max_objetos",
    "min_variaveis", "media_variaveis", "max_variaveis"
]

# Gera as linhas em LaTeX
linhas = []
for _, row in df[colunas].iterrows():
    linha = (
        f"{row['datalake']} & "
        f"{int(row['num_datasets'])} & "
        f"{int(row['min_objetos'])} & "
        f"{row['media_objetos']:.2f} & "
        f"{int(row['max_objetos'])} & "
        f"{int(row['min_variaveis'])} & "
        f"{row['media_variaveis']:.2f} & "
        f"{int(row['max_variaveis'])} \\\\"
    )
    linhas.append(linha)

# Exibe as linhas no console
print("\n".join(linhas))
