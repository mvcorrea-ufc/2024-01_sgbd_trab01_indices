#include "hash_index.h"

// conditional debuggin
#define DEBUG 1

// Cria um novo diretório de hash com profundidade inicial e arquivos de bucket
HashDirectory *create_hash_directory(int initial_depth) {
    // system("rm -rf buckets/*");     // limpa a pasta de buckets
    HashDirectory *dir = (HashDirectory *)malloc(sizeof(HashDirectory));
    dir->global_depth = initial_depth;
    dir->next_bucket_id = 0;
    int num_buckets = 1 << initial_depth;  // 2^initial_depth
    dir->buckets = (Bucket **)malloc(num_buckets * sizeof(Bucket *));
    for (int i = 0; i < num_buckets; i++) {
        dir->buckets[i] = (Bucket *)malloc(sizeof(Bucket));
        dir->buckets[i]->local_depth = initial_depth;
        dir->buckets[i]->num_entries = 0;
        // Inicializar o nome do arquivo do bucket
        sprintf(dir->buckets[i]->filename, "buckets/bucket%d.txt", i);
        if(DEBUG) printf(">>\tcreate_hash_directory: Arquivo do bucket %d: %s\n", i, dir->buckets[i]->filename);
        
        // Criar/Reiniciar o arquivo para o bucket
        FILE *bucket_file = fopen(dir->buckets[i]->filename, "w");
        if (bucket_file == NULL) {
            perror("Erro ao criar o arquivo do bucket");
            exit(EXIT_FAILURE);
        }
        fclose(bucket_file);

        if(i >20) exit(0);
    }
    return dir;
}

// Função hash para calcular o índice do bucket
int hash_function(int key, int depth) {
    return key & ((1 << depth) - 1);  // Retorna os 'depth' bits menos significativos de 'key'
}

// Função auxiliar para extrair a chave de uma entrada
int extract_key(const char *entry) {
    int key;
    sscanf(entry, "%d,", &key);
    return key;
}

// Carrega dados iniciais do arquivo CSV para o índice hash
void load_data_from_csv(const char *filename, HashDirectory *dir) {
    FILE *csvfile = fopen(filename, "r");
    if (csvfile == NULL) {
        perror("Erro ao abrir o arquivo CSV");
        return;
    }

    if(DEBUG) printf("\n> load_data_from_csv: Carregando dados do arquivo CSV...\n");
    char line[1024];
    while (fgets(line, sizeof(line), csvfile) != NULL) {
        // clean
        int pedido, ano, order;
        float valor;
        char data[1000];

        // Supondo que cada linha no CSV seja no formato: chave,dados
        if (sscanf(line, "%d,%s", &order, data) == 2) {
            if(DEBUG) printf(">>\tload_data_from_csv: Pedido: %d, Dados: %s\n", order, data);
            insert_entry(dir, order, data);
        }
    }

    fclose(csvfile);
}

// Insere uma entrada no índice hash
void insert_entry(HashDirectory *dir, int key, char *data) {
        
        int index = hash_function(key, dir->global_depth);
        Bucket *bucket = dir->buckets[index];

        // Caminho para o arquivo do bucket
        char bucket_path[256];
        sprintf(bucket_path, "buckets/bucket%d.txt", index);
        printf(">>>\tinsert_entry: Chave: %d, Dados: %s, Hash: %d, file: %s\n", key, data, index, bucket_path);

        FILE *bucket_file = fopen(bucket_path, "a"); // Abrir o arquivo para adicionar dados
        if (bucket_file == NULL) {
            perror("Erro ao abrir o arquivo do bucket");
            return;
        }

        if (bucket->num_entries < 3) {
                // split data (pedido, data)
                fprintf(bucket_file, "%d,%s\n", key, data);
                bucket->num_entries++;
        } else {
            // TODO: Implementar a divisão do bucket
            printf("\n>>>\tinsert_entry: Bucket %d está cheio, dividindo...\n", index);
            split_bucket(dir, index);
            insert_entry(dir, key, data); // Tentar novamente após a divisão
        }

        fclose(bucket_file); // Fechar o arquivo após a inserção
        fflush(stdout);
}



// Busca por uma entrada usando a chave
char *search_entry(HashDirectory *dir, int key) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    
    // Implementação da lógica de busca
    return NULL;
}

// Remove uma entrada do índice hash
void delete_entry(HashDirectory *dir, int key) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    
    // Implementação da lógica de remoção
}

// Duplica o diretório quando necessário
void double_directory(HashDirectory *dir) {
    // Implementação do redimensionamento do diretório
}

// Divide um bucket específico
void split_bucket(HashDirectory *dir, int bucket_index) {
    Bucket *bucket = dir->buckets[bucket_index];            // Bucket a ser dividido
    int old_depth = bucket->local_depth;
    int new_depth = old_depth + 1;
    int new_bucket_index = bucket_index + (1 << old_depth); // Novo índice do bucket (adicionado um bit mais significativo)

    printf(">>>>\tsplit_bucket: Bucket: %d, Profundidade local: %d, Novo índice: %d\n", bucket_index, bucket->local_depth, new_bucket_index);

    //if(DEBUG) if(new_bucket_index > 50) exit(0);

    // Criar o novo bucket com profundidade local atualizada
    dir->buckets[new_bucket_index] = (Bucket *)malloc(sizeof(Bucket)); // Alocar memória para o novo bucket
    dir->buckets[new_bucket_index]->local_depth = bucket->local_depth;
    dir->buckets[new_bucket_index]->num_entries = 0;
    sprintf(dir->buckets[new_bucket_index]->filename, "buckets/bucket%d.txt", new_bucket_index);
    printf(">>>>\tsplit_bucket: Novo bucket: %s\n", dir->buckets[new_bucket_index]->filename);

    FILE *new_bucket_file = fopen(dir->buckets[new_bucket_index]->filename, "w");
    if (new_bucket_file == NULL) {
        perror("Erro ao criar o arquivo do novo bucket");
        return;
    }
    fclose(new_bucket_file);

    // Incrementar a profundidade local do bucket original
    bucket->local_depth++;

    // Incrementar a profundidade global, se necessário
    if (new_depth > dir->global_depth) {
        dir->global_depth = new_depth;
    } 

    // Reabrir o arquivo do bucket original e criar um buffer temporário para as entradas
    FILE *original_bucket_file = fopen(bucket->filename, "r+");
    char line[256];
    char *temp_entries[10];  // Supondo um número maior temporariamente para re-hashing
    int temp_count = 0;

    while (fgets(line, sizeof(line), original_bucket_file) != NULL) {
        line[strcspn(line, "\n")] = 0;              // Remover o caractere de nova linha
        temp_entries[temp_count++] = strdup(line);  // Copiar linha para buffer temporário
    }

    // Redefinir o arquivo do bucket original
    freopen(bucket->filename, "w", original_bucket_file);

    printf(">>>>\tsplit_bucket: Entradas distribuidas por [%d] e [%d]\n", bucket_index, new_bucket_index );

    // Redistribuir as entradas
    // CHECK: O que acontece se a chave for a mesma?
    for (int i = 0; i < temp_count; i++) {
        int key;
        //sscanf(temp_entries[i], "%d,%*s", &key);
        sscanf(temp_entries[i], "%d,%*s", &key);
        int index = hash_function(key, dir->global_depth);
        printf(">>>>\tsplit_bucket: Chave: %d, Dados: %s, Hash: %d << ", key, temp_entries[i], index);

        if (index == bucket_index) {
            strcat(temp_entries[i], "\n");
            fputs(temp_entries[i], original_bucket_file);
            printf("Mantendo no bucket original [%d] >>\n", bucket_index);
        } else {
            FILE *new_file = fopen(dir->buckets[new_bucket_index]->filename, "a");
            strcat(temp_entries[i], "\n");
            fputs(temp_entries[i], new_file);
            fclose(new_file);
            printf("Movendo para o novo bucket [%d] >>\n", new_bucket_index);
        }
        free(temp_entries[i]);  // Liberar a memória alocada para as entradas temporárias
    }

    fclose(original_bucket_file);
}

// Libera toda a memória alocada para o diretório hash
void free_hash_directory(HashDirectory *dir) {
    // Implementação para liberar a memória
}

