#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "directory_entry.h"

#define N_INODE_BLOCK_ADDRESSES 10
#define MAX_FILESIZE (BLOCK_SIZE * ((N_INODE_BLOCK_ADDRESSES-1) + N_BLOCK_ADDRESSES)) // tamanho maximo de arquivo suportado pelo simulador

typedef struct {
    char filename[MAX_FILENAME_SIZE]; // Nome do arquivo com tamanho fixo
    int permissions; // Permissões do arquivo são um inteiro que será tratado como binário de acordo com o padrão UNIX
    int size; // Tamanho em bytes
    bool is_directory; // Indica se é um diretório ou não
    time_t created_at; // Criado em...
    time_t modified_at; // Modificado em...
    time_t last_accessed_at; // Acessado em...
    int32_t block_addresses[N_INODE_BLOCK_ADDRESSES]; // Endereços de blocos
} INode;

// Função para inicializar um Inode
void inode_init(INode *inode);

#endif
