#ifndef PARTICAO_H
#define PARTICAO_H

#include <stddef.h>
#include <stdint.h>

#include "inode.h"
#include "block.h"

// tamanho maximo de arquivo suportado pelo simulador, tem esse limite pois
// este simulador tem no maximo um bloco indireto, que é o bloco indireto do inode
#define MAX_FILESIZE (BLOCK_SIZE * (N_DIRECT_BLOCKS + N_BLOCK_ADDRESSES))

/*
intervalos definidos:
    - tamanho partição: 10MB a 100MB (inclusivo)
    - tamanho bloco: 1KB a 4KB (inclusivo)
    - tamanho maximo de arquivo: MAX_FILESIZE
tamanho de um endereço de bloco: 4bytes (int32_t)
*/

/*
macros uteis:
    N_INODES: numero de blocos para inodes
    N_DATA_BLOCKS: numero de blocos para dados
*/

/*
Considerando que um inode ocupa um bloco, uma particao é composta por blocos
com inodes e blocos de dados. O numero total de blocos então é dividido para
os inodes e para os blocos de dados.
*/
typedef struct {
    INode inodes[N_INODES]; // vetor dos inodes
    Block data_blocks[N_DATA_BLOCKS]; // vetor dos blocos de dados
    uint8_t free_blocks_bitmap[N_DATA_BLOCKS]; // mapa de bits para controlar os blocos livres: 0 = livre, 1 = ocupado
    int32_t n_free_blocks; // numero de blocos livres
} Partition;

void partition_init(Partition *partition);
bool partition_create_file(Partition *partition, char *filename);
#endif
