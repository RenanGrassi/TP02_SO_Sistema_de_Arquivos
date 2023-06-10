#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FILENAME_SIZE 256
#define MAX_BLOCK_POINTERS 10

typedef struct {
    char filename[MAX_FILENAME_SIZE]; // Nome do arquivo com tamanho fixo
    int permissions; // Permissões do arquivo são um inteiro que será tratado como binário de acordo com o padrão UNIX
    int size; // Tamanho em bytes
    bool is_directory; // Indica se é um diretório ou não
    time_t created_at; // Criado em...
    time_t modified_at; // Modificado em...
    time_t last_accessed_at; // Acessado em...
    int32_t direct_blocks[MAX_BLOCK_POINTERS]; // Endereços de blocos diretos
    int32_t indirect_block; // Endereço de bloco indireto para referenciar mais blocos de dados, se necessário
} INode;

/*
    O campo indirect_block permite que um i-node referencie um bloco indireto, que por sua vez contém mais referências
    a blocos de dados. Essa abordagem indireta é usada para suportar arquivos maiores que o número máximo de blocos
    diretamente referenciados pelos data_blocks. Faltam outras estruturas de dados, como
    estruturas de diretório, uma tabela de i-nodes, um mapa de bits para rastrear blocos livres, além de funções
    para manipulação de arquivos e diretórios.
*/

// Função para inicializar um Inode
void createRootDirectory(INode* inode, char* filename);

void createDirectory(INode* inode, char* filename);

void renameDirectory(INode* inode, char* filename);

void createFile(INode* inode, char* filename);

#endif
