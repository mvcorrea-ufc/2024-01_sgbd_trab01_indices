#include "hash_index.h"

// conditional debuggin
#define DEBUG 1
#define CRESET  "\x1b[0m"
#define CCYAN   "\x1b[36m"
#define CRED    "\x1b[31m"


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

    if(DEBUG) printf(CCYAN "\n> load_data_from_csv: Carregando dados do arquivo CSV...\n" CRESET);
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
int insert_entry(HashDirectory *dir, int key, char *data) {
        
        int index = hash_function(key, dir->global_depth);
        Bucket *bucket = dir->buckets[index];

        // Caminho para o arquivo do bucket
        char bucket_path[256];
        sprintf(bucket_path, "buckets/bucket%d.txt", index);
        printf(">>>\tinsert_entry: Chave: %d, Dados: %s, Hash: %d, file: %s\n", key, data, index, bucket_path);

        FILE *bucket_file = fopen(bucket_path, "a"); // Abrir o arquivo para adicionar dados
        if (bucket_file == NULL) {
            perror("Erro ao abrir o arquivo do bucket");
            return 0;
        }

        if (bucket->num_entries < 3) {
            // split data (pedido, data)
            fprintf(bucket_file, "%d,%s\n", key, data);
            bucket->num_entries++;
        } else {
            printf("\n>>>\tinsert_entry: Bucket %d está cheio, dividindo...\n", index);
            split_bucket(dir, index);
            insert_entry(dir, key, data); // Tentar novamente após a divisão
        }

        fclose(bucket_file); // Fechar o arquivo após a inserção
        fflush(stdout);
        return bucket->local_depth;
}



// Busca por uma entrada usando a chave !TODO!
char *search_entry(HashDirectory *dir, int key) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    
    // Implementação da lógica de busca
    return NULL;
}

// Remove uma entrada do índice hash
int delete_entry(HashDirectory *dir, int key) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    printf(">>>\tdelete_entry: Removendo chave: %d do bucket: %d\n", key, index);

    // Abrir o arquivo do bucket para leitura
    FILE *file = fopen(bucket->filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo do bucket");
        return 0;
    }

    char line[1024];
    char *new_contents = NULL; // Buffer para armazenar conteúdo atualizado
    size_t new_size = 0;

    // Ler o arquivo e filtrar a entrada a ser removida
    while (fgets(line, sizeof(line), file) != NULL) {
        int current_key;
        sscanf(line, "%d,%*s", &current_key);
        if (current_key != key) {
            // Se a chave não é a que queremos deletar, adiciona ao buffer
            new_size += strlen(line);
            new_contents = realloc(new_contents, new_size + 1);
            strcat(new_contents, line);
        }
        printf(">>>\tdelete_entry: Lendo linha: %s\t%s", line);
    }

    fclose(file);

    // Reescrever o arquivo do bucket apenas com as entradas que não foram deletadas
    file = fopen(bucket->filename, "w");
    if (file == NULL) {
        perror("Erro ao reabrir o arquivo do bucket para escrita");
        free(new_contents);
        return 0;
    }

    if (new_contents) {
        fprintf(file, "%s", new_contents);
        free(new_contents);
    }

    fclose(file);
    return bucket->local_depth;
}

// Duplica o diretório quando necessário !TODO!
void double_directory(HashDirectory *dir) {
    // Implementação do redimensionamento do diretório
}

// Divide um bucket específico
void split_bucket(HashDirectory *dir, int bucket_index) {
    Bucket *bucket = dir->buckets[bucket_index];            // Bucket a ser dividido
    int old_depth = bucket->local_depth;
    int new_depth = old_depth + 1;
    int new_bucket_index = bucket_index + (1 << old_depth); // Novo índice do bucket (adicionado um bit mais significativo)

    printf("*>>>\tsplit_bucket: Dividindo o bucket: %d\n", bucket_index);
    printf("*>>>\tsplit_bucket: Profundidade global: %d, Profundidade local: %d\n", dir->global_depth, old_depth);
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

    // Reinicializar o bucket original
    bucket->num_entries = 0;

    // Redistribuir as entradas
    for (int i = 0; i < temp_count; i++) {
        int key;
        //sscanf(temp_entries[i], "%d,%*s", &key);
        sscanf(temp_entries[i], "%d,%*s", &key);
        int index = hash_function(key, dir->global_depth);
        printf(">>>>\tsplit_bucket: Chave: %d, Dados: %s, Hash: %d << ", key, temp_entries[i], index);

        if (index == bucket_index) {
            strcat(temp_entries[i], "\n");
            fputs(temp_entries[i], original_bucket_file);
            bucket->num_entries++;
            printf("Mantendo no bucket original [%d] >>\n", bucket_index);
        } else {
            FILE *new_file = fopen(dir->buckets[new_bucket_index]->filename, "a");
            strcat(temp_entries[i], "\n");
            fputs(temp_entries[i], new_file);
            fclose(new_file);
            dir->buckets[new_bucket_index]->num_entries++;
            printf("Movendo para o novo bucket [%d] >>\n", new_bucket_index);
        }

        free(temp_entries[i]);  // Liberar a memória alocada para as entradas temporárias
    }

    fclose(original_bucket_file);
}

// Libera a memória alocada para um bucket
void free_bucket(Bucket *bucket) {
    if(bucket) {
        for(int i = 0; i < bucket->num_entries; i++) {
            printf(">>\tfree_bucket: liberando a memoria de [%s][%d] entradas\n",bucket->filename, bucket->num_entries);
            printf(">>\tfree_bucket: liberando a memoria de indice [%d] [%s]\n", i, bucket->entries[i]);
            free(bucket->entries[i]);
        }
        //free(bucket->entries);
        //free(bucket);
    }
}

// Libera toda a memória alocada para o diretório hash
void free_hash_directory(HashDirectory *dir) {
    if(dir) {
        int global_depth = 1 << dir->global_depth;
        printf(">>\tfree_hash_directory: liberando a memoria de [%d] buckets in dir\n", global_depth);
        for(int i = 0; i < global_depth; i++) {
            printf(">>\tfree_hash_directory: liberando a memoria de [%s] com [%d] entrada\n",dir->buckets[i]->filename, dir->buckets[i]->num_entries);
            free_bucket(dir->buckets[i]);
        }
        free(dir->buckets);
        free(dir);
    }
}

