#ifndef DIRECTORY_ENTRY_H
#define DIRECTORY_ENTRY_H

#include <stdint.h>

#define MAX_FILENAME_SIZE 256 // tamanho maximo nome de arquivo
#define DIR_ENTRY_SIZE (MAX_FILENAME_SIZE + BLOCK_ADDRESS_SIZE) // tamanho de uma entrada de diretorio
#define N_DIR_ENTRIES (BLOCK_SIZE / DIR_ENTRY_SIZE) // numero de entradas de diretorio que cabem em um bloco

typedef struct {
    char filename[MAX_FILENAME_SIZE]; // nome do arquivo
    uint32_t inode_address; // endereco do inode
} DirectoryEntry;

void dir_entry_init(DirectoryEntry *dir_entry, char *filename, uint32_t inode_address);

#endif
