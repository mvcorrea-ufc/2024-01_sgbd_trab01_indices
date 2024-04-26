#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura de um bucket
typedef struct {
    int local_depth;  // Profundidade local do bucket
    int num_entries;  // Número atual de entradas no bucket
    char **entries;   // Entradas armazenadas no bucket
    char filename[256]; // Caminho para o arquivo do bucket
} Bucket;

// Estrutura do diretório do índice hash
typedef struct {
    int global_depth; // Profundidade global do diretório
    int next_bucket_id; // Próximo ID de bucket a ser criado
    Bucket **buckets; // Ponteiros para os buckets
} HashDirectory;

// Funções do índice hash
HashDirectory *create_hash_directory(int initial_depth);
int hash_function(int key, int depth);
int extract_key(const char *entry);
void load_data_from_csv(const char *filename, HashDirectory *dir);
int insert_entry(HashDirectory *dir, int key, char *data);
char *search_entry(HashDirectory *dir, int key);
int delete_entry(HashDirectory *dir, int key);
void double_directory(HashDirectory *dir);
void split_bucket(HashDirectory *dir, int bucket_index);
void free_bucket(Bucket *bucket);
void free_hash_directory(HashDirectory *dir);

#endif

