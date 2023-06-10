#ifndef PARTICAO_H
#define PARTICAO_H

#include <stddef.h>

#include "inode.h"
#include "block.h"

typedef struct {
    INode inodes[N_INODES];
    Block data_blocks[N_DATA_BLOCKS];
} Partition;


#endif

/*
tamanho partição: 10MB a 100MB
tamanho bloco: 1KB a 4KB
tamanho de um endereço de bloco: 4bytes
*/
