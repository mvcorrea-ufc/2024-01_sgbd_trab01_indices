#include "hash_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// debug
#define DEBUG 1
#define CRESET  "\x1b[0m"
#define CCYAN   "\x1b[36m"
#define CRED    "\x1b[31m"
#define CGREEN  "\x1b[32m"

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
    int initial_depth = 0;
    fscanf(infile, "PG/%d\n", &initial_depth);
    if(initial_depth == 0) {
        fprintf(stderr, "Erro: Profundidade global inválida\n");
        fclose(infile);
        fclose(outfile);
        return EXIT_FAILURE;
    }
    if(DEBUG) printf(CCYAN "\n> main: Profundidade global inicial: %d\033[0m\n" CRESET, initial_depth);
    
    // Criar o diretório hash com a profundidade inicial
    HashDirectory *dir = create_hash_directory(initial_depth);
    
    //exit(0);

    // Carregar dados iniciais do arquivo CSV
    load_data_from_csv("data/compras.csv", dir);

    // Processar as operações no arquivo de entrada
    char operation[10], data[20];
    int key;

    //fflush(stdout);

    if(DEBUG) printf(CCYAN "\n> main: Processando operações...\n" CRESET);

    int cnt = 0;
    
    while (fscanf(infile, "\n%[^:]:%d,%s", operation, &key, data) != EOF) {
        //operation[strcspn(operation,"\n")] = '\0';

        if(strcmp(operation, "INC") == 0 || strcmp(operation, "REM") == 0 || strcmp(operation, "BUS=") == 0) {
            //printf("[%s][%d][%s]\n", operation, key, data);
            if(DEBUG) printf(CGREEN "\n>>\tmain: op:[%s], key:[%d], data[%s]\n" CRESET, operation, key, data);

            if(strcmp(operation, "INC") == 0 && data[0] != '\0') {
                int local_depth = insert_entry(dir, key, data);
                fprintf(outfile, "INC:%d/%d,%d\n", key, dir->global_depth, local_depth);
            } 
            else if(strcmp(operation, "REM") == 0) {
                int local_depth = delete_entry(dir, key);
                fprintf(outfile, "REM:%d/,%d,%d\n", key, dir->global_depth, local_depth);
            } 
            /*
            else if(strcmp(operation, "BUS") == 0) {
                char *result = search_entry(dir, key);
                if (result != NULL) {
                    fprintf(outfile, "BUS:%d/Encontrado\n", key);
                } else {
                    fprintf(outfile, "BUS:%d/Não encontrado\n", key);
                }
            }
            */

        } else {
            fprintf(stderr, CRED "\nErro: Operação inválida [%s]\n" CRESET, operation);
            continue;
        }

        // reset data
        memset(operation, 0, sizeof(operation));
        memset(data, 0, sizeof(data));
        memset(&key, 0, sizeof(key));       
    }

    // Liberar recursos e fechar arquivos
    //free_hash_directory(dir);
    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}

