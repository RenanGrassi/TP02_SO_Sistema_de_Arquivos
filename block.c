#include "block.h"
#include "directory_entry.h"
#include <stdint.h>
#include <string.h>

void block_init(Block *block) {
    // inicializa o bloco com zeros
    memset(block->data, 0, BLOCK_SIZE * sizeof(char));
}

// retorna o endereco em um certo indice
int32_t block_read_address(Block block, int index) {
    int32_t address = *((int32_t*)(block.data + (index*BLOCK_ADDRESS_SIZE)));
    return address;
}

// escreve o endereco em um certo indice
void block_write_address(Block *block, int index, int32_t address) {
    *((int32_t*)(block->data + (index*BLOCK_ADDRESS_SIZE))) = address;
}

DirectoryEntry block_read_dir_entry(Block block, int index) {
    DirectoryEntry dir_entry = *((DirectoryEntry*)(block.data + (index*DIR_ENTRY_SIZE)));
    return dir_entry;
}

void block_write_dir_entry(Block *block, int index, DirectoryEntry dir_entry) {
    *((DirectoryEntry*)(block->data + (index*DIR_ENTRY_SIZE))) = dir_entry;
}
