#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

/*
macros uteis:
    BLOCK_SIZE: tamanho de um bloco em bytes
    BLOCK_ADDRESS_SIZE: tamanho de um bloco
    N_BLOCK_ADDRESSES: numero de enderecos que cabem em um bloco
*/


typedef struct {
    // char é 1 byte, logo um vetor de char com tamanho de um bloco vai ocupar exatamente um bloco
    char data[BLOCK_SIZE];
} Block;

void block_init(Block *block);
int32_t block_read_address(Block block, int index);
void block_write_address(Block *block, int index, int32_t address);

#endif
