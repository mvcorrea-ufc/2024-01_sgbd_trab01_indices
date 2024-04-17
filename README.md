## 2024-01_sgbd_trab01_indices
primeiro trbalho cadeira de sgbd, UFC 2024-04.

### Estrutura:
```
sgbd_trab01_indices/
├── src/
│   ├── main.c              # Arquivo principal que inclui a lógica de manipulação de entrada/saída.
│   ├── hash_index.c        # Implementação das funções de índice hash (inclusão, remoção, busca).
│   └── hash_index.h        # Definições de estruturas e protótipos de funções para hash_index.c.
├── data/
│   ├── compras.csv         # Dados de exemplo para o índice.
│   ├── in.txt              # Arquivo de entrada com operações a serem realizadas.
│   └── out.txt             # Arquivo de saída com os resultados das operações.
├── buckets/
│   ├── bucket0.txt         # Arquivos de bucket para armazenamento dos dados indexados.
│   ├── bucket1.txt
│   └── ...
├── docs/
│   └── 2024_CK0117_CK0446_SGBD_Trabalho_Indices.pdf 
├── Makefile                # Script de automação para compilar e executar o projeto.
└── README.md               # Instruções e informações sobre o projeto.
```
### Exemplo:
```
PG/3
INC:1001
INC:2002
INC:3003
BUS=:1001
REM:2002
BUS=:2002
INC:4004
BUS=:4004

```
Descrição:
```
PG/3: Configura a profundidade global inicial do índice hash como 3. Isso significa que o diretório começará com 2^3 (ou seja, 8) buckets potenciais.
INC:1001: Insere um registro com chave 1001.
INC:2002: Insere um registro com chave 2002.
INC:3003: Insere um registro com chave 3003.
BUS=:1001: Busca pelo registro com chave 1001 e espera encontrar o resultado.
REM:2002: Remove o registro com chave 2002.
BUS=:2002: Busca pelo registro com chave 2002, mas não deve encontrar, pois foi removido.
INC:4004: Insere um novo registro com chave 4004.
BUS=:4004: Busca pelo registro com chave 4004 e espera encontrar o resultado.
```







