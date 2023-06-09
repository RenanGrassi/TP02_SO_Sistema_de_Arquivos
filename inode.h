#include <time.h>

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
partição[0] = inode os endereços do dado (3 data_blocks)