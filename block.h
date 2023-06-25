#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

#include "directory_entry.h"

#define BLOCK_ADDRESS_SIZE 4    // tamanho de endereço de bloco em bytes
#define N_BLOCK_ADDRESSES (BLOCK_SIZE / BLOCK_ADDRESS_SIZE) // numero de enderecos de bloco que cabem em um bloco
#define N_DIR_ENTRIES (BLOCK_SIZE / DIR_ENTRY_SIZE) // numero de entradas de diretorio que cabem em um bloco

typedef struct {
    char data[BLOCK_SIZE];
} Block;

void block_init(Block *block);
int32_t block_read_address(Block block, int index);
void block_write_address(Block *block, int index, int32_t address);
DirectoryEntry block_read_dir_entry(Block block, int index);
void block_write_dir_entry(Block *block, int index, DirectoryEntry dir_entry);

#endif
