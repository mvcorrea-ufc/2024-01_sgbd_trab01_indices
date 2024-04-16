#include "hash_index.h"

/* // Cria um novo diretório de hash com profundidade inicial
HashDirectory *create_hash_directory(int initial_depth) {
    HashDirectory *dir = (HashDirectory *)malloc(sizeof(HashDirectory));
    dir->global_depth = initial_depth;
    int num_buckets = 1 << initial_depth;  // 2^initial_depth
    dir->buckets = (Bucket **)malloc(num_buckets * sizeof(Bucket *));
    for (int i = 0; i < num_buckets; i++) {
        dir->buckets[i] = (Bucket *)malloc(sizeof(Bucket));
        dir->buckets[i]->local_depth = initial_depth;
        dir->buckets[i]->num_entries = 0;
        dir->buckets[i]->entries = (char **)malloc(3 * sizeof(char *));  // Supondo um máximo de 3 entradas por bucket
    }
    return dir;
} */

// Cria um novo diretório de hash com profundidade inicial e arquivos de bucket
HashDirectory *create_hash_directory(int initial_depth) {
    // system("rm -rf buckets/*");     // limpa a pasta de buckets
    HashDirectory *dir = (HashDirectory *)malloc(sizeof(HashDirectory));
    dir->global_depth = initial_depth;
    int num_buckets = 1 << initial_depth;  // 2^initial_depth
    dir->buckets = (Bucket **)malloc(num_buckets * sizeof(Bucket *));
    for (int i = 0; i < num_buckets; i++) {
        dir->buckets[i] = (Bucket *)malloc(sizeof(Bucket));
        dir->buckets[i]->local_depth = initial_depth;
        dir->buckets[i]->num_entries = 0;
        // Inicializar o nome do arquivo do bucket
        sprintf(dir->buckets[i]->filename, "buckets/bucket%d.txt", i);
        printf("> Arquivo do bucket %d: %s\n", i, dir->buckets[i]->filename);
        
        // Criar/Reiniciar o arquivo para o bucket
        FILE *bucket_file = fopen(dir->buckets[i]->filename, "w");
        if (bucket_file == NULL) {
            perror("Erro ao criar o arquivo do bucket");
            exit(EXIT_FAILURE);
        }
        fclose(bucket_file);
    }
    return dir;
}

// Função hash para calcular o índice do bucket
int hash_function(int key, int depth) {
    return key & ((1 << depth) - 1);  // Retorna os 'depth' bits menos significativos de 'key'
}

// Insere uma entrada no índice hash
void insert_entry(HashDirectory *dir, int key, char *data) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];

    // Caminho para o arquivo do bucket
    char bucket_path[256];
    sprintf(bucket_path, "buckets/bucket%d.txt", index);

    FILE *bucket_file = fopen(bucket_path, "a"); // Abrir o arquivo para adicionar dados
    if (bucket_file == NULL) {
        perror("Erro ao abrir o arquivo do bucket");
        return;
    }

    if (bucket->num_entries < 3) {
        fprintf(bucket_file, "%s\n", data);
        bucket->num_entries++;
    } else {
        // Lidar com o caso de bucket cheio, como dividir o bucket
    }

    fclose(bucket_file); // Fechar o arquivo após a inserção
}


/* void insert_entry(HashDirectory *dir, int key, char *data) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    
    // Implementação da lógica de inserção (incluindo split e double)
} */

// Busca por uma entrada usando a chave
char *search_entry(HashDirectory *dir, int key) {
    int index = hash_function(key, dir->global_depth);
    Bucket *bucket = dir->buckets[index];
    
    // Implementação da lógica de busca
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
    // Implementação da divisão do bucket
}

// Libera toda a memória alocada para o diretório hash
void free_hash_directory(HashDirectory *dir) {
    // Implementação para liberar a memória
}

