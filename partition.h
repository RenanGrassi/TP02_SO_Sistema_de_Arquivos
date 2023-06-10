#ifndef PARTICAO_H
#define PARTICAO_H

#include <stddef.h>

#include "inode.h"
#include "block.h"

/*
intervalos definidos:
    - tamanho partição: 10MB a 100MB (inclusivo)
    - tamanho bloco: 1KB a 4KB (inclusivo)
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
} Partition;


#endif
