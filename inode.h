#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "directory_entry.h"

#define N_INODE_BLOCK_ADDRESSES 10

typedef struct {
    char filename[MAX_FILENAME_SIZE]; // Nome do arquivo com tamanho fixo
    int permissions; // Permissões do arquivo são um inteiro que será tratado como binário de acordo com o padrão UNIX
    int size; // Tamanho em bytes
    bool is_directory; // Indica se é um diretório ou não
    time_t created_at; // Criado em...
    time_t modified_at; // Modificado em...
    time_t last_accessed_at; // Acessado em...

    /* address_blocks armazena endereços de blocos diretos,
       exceto o ultimo endereço, que pode ser um endereço de bloco indireto:
        - inode de arquivo:
            - ultimo endereço é usado exclusivamente como endereço de bloco indireto.
            - dentro desse bloco indireto, são armazenados apenas endereços de blocos diretos.
        - inode de diretorio:
            - ultimo endereço é usado como endereço de bloco direto */
    int32_t address_blocks[N_INODE_BLOCK_ADDRESSES]; // Endereços de blocos
} INode;

// Função para inicializar um Inode
void inode_init(INode *inode);



// void createRootDirectory(INode* inode, char* filename);

// void createDirectory(INode* inode, char* filename);

// void renameDirectory(INode* inode, char* filename);

// void createFile(INode* inode, char* filename);

#endif
