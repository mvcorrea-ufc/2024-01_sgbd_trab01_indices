#include "hash_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Abrir o arquivo de entrada
    FILE *infile = fopen("data/in.txt", "r");
    if (infile == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return EXIT_FAILURE;
    }

    // Abrir o arquivo de saída
    FILE *outfile = fopen("data/out.txt", "w");
    if (outfile == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(infile);
        return EXIT_FAILURE;
    }

    // Ler a profundidade global inicial do diretório de hash
    int initial_depth;
    fscanf(infile, "PG/%d\n", &initial_depth);
    printf("> Profundidade global inicial: %d\n", initial_depth);

    // Criar o diretório hash com a profundidade inicial
    HashDirectory *dir = create_hash_directory(initial_depth);

    // Processar as operações no arquivo de entrada
    char operation[10];
    int key;
    while (fscanf(infile, "%s:%d\n", operation, &key) == 2) {
        printf("> Operação: %s, Chave: %d\n", operation, key);
        if (strcmp(operation, "INC") == 0) {
            insert_entry(dir, key, "alguma_data"); // Suponha que a entrada de dados seja uma string genérica
            fprintf(outfile, "INC:%d/Operação concluída\n", key);
        } else if (strcmp(operation, "REM") == 0) {
            delete_entry(dir, key);
            fprintf(outfile, "REM:%d/Operação concluída\n", key);
        } else if (strcmp(operation, "BUS") == 0) {
            char *result = search_entry(dir, key);
            if (result != NULL) {
                fprintf(outfile, "BUS:%d/Encontrado\n", key);
            } else {
                fprintf(outfile, "BUS:%d/Não encontrado\n", key);
            }
        }
    }

    // Liberar recursos e fechar arquivos
    free_hash_directory(dir);
    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}

