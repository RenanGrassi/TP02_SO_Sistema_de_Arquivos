#ifndef INODE_H
#define INODE_H

#include <stdint.h>
#include <time.h>
#include <string.h>

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
    int direct_blocks[MAX_BLOCK_POINTERS]; // Endereços de blocos diretos
    int indirect_block; // Endereço de bloco indireto para referenciar mais blocos de dados, se necessário
}INode;

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

/*#include <time.h>
#define MAX_BLOCKS 10 // Número máximo de blocos de dados
typedef struct data_block * dtb;

typedef struct {
    char filename[256];     // Nome do arquivo
    int size;               // Tamanho do arquivo
    time_t created_at;      // Data/hora de criação
    time_t modified_at;     // Data/hora da última modificação
    int permissions;         // Um inteiro que vai até 512 que é convertido para binario quando necessitamos das permissoes
    int data_blocks[10]; // Blocos de dados do arquivo
    dtb // chama a funcao pra alocar o datablock
} Inode;

typedef struct block{
    int data_blocks[MAX_BLOCKS];
}block;

//Neste exemplo, a estrutura de dados Inode possui os seguintes campos:

//filename: uma string para armazenar o nome do arquivo.
//size: um inteiro para armazenar o tamanho do arquivo em bytes.
//created_at: uma variável do tipo time_t para armazenar a data/hora de criação do arquivo.
//modified_at: uma variável do tipo time_t para armazenar a data/hora da última modificação do arquivo.
//permissions: um inteiro para armazenar as permissões do arquivo.
//data_blocks: uma matriz de inteiros para armazenar os números dos blocos de dados do arquivo.
//Você pode adicionar métodos ou funções auxiliares para manipular a estrutura de dados do Inode, como funções para definir o tamanho do arquivo, definir as permissões, adicionar/remover blocos de dados, etc. Lembre-se de que este é apenas um exemplo básico de estrutura de dados de um Inode e você pode expandi-la ou modificá-la de acordo com as necessidades específicas do seu simulador de sistema de arquivos.


gerenciador de partiçao tem lista 10.000
partição[0] = inode os endereços do dado (3 data_blocks)*/
