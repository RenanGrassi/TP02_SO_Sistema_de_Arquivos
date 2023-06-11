#include "partition.h"
#include "block.h"
#include "directory_entry.h"
#include "inode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>

void partition_init(Partition *partition) {
    for (int i = 0; i < N_DATA_BLOCKS; i++) {
        block_init(&partition->data_blocks[i]);
    }
    for (int i = 0; i < N_INODES; i++) {
        inode_init(&partition->inodes[i]);
    }
    // inicializa com zeros
    memset(partition->free_blocks_bitmap, 0, N_DATA_BLOCKS * sizeof(int8_t));
    partition->n_free_blocks = N_DATA_BLOCKS;
}

// retorna o endereco de um bloco livre, e retorna -1 se nao achou
static int32_t partition_find_free_block(Partition *partition) {
    for (int32_t i = 0; i < N_DATA_BLOCKS; i++) {
        if (partition->free_blocks_bitmap[i] == 0) {
            return i;
        }
    }
    return -1;
}

// retorna o endereco de um inode livre, e retorna -1 se nao achou
static int32_t partition_find_free_inode(Partition *partition) {
    for (int32_t i = 0; i < N_INODES; i++) {
        if (strlen(partition->inodes[i].filename) == 0) {
            return i;
        }
    }
    return -1;
}

// insere uma entrada de diretorio em um inode de diretorio
bool partition_insert_dir_entry(Partition *partition, INode *dir_inode, DirectoryEntry dir_entry) {
    if (!dir_inode->is_directory) {
        return false;
    }

    int32_t block_address = -1;
    for (int i = 0; i <= N_DIRECT_BLOCKS; i++) {
        // se nao achar um espaco vazio nos blocos diretos, procura no bloco indireto
        if (i == N_DIRECT_BLOCKS) {
            block_address = dir_inode->indirect_block;
            if (block_address == -1) {
                block_address = partition_find_free_block(partition);
                if (block_address == -1) {
                    return false;
                }

                dir_inode->indirect_block = block_address;
            }
        } else {
            block_address = dir_inode->direct_blocks[i];
            if (block_address == -1) {
                block_address = partition_find_free_block(partition);
                if (block_address == -1) {
                    return false;
                }

                dir_inode->direct_blocks[i] = block_address;
            }
        }



        // procura um lugar vazio para escrever a entrada de diretorio
        for (int j = 0; j < N_DIR_ENTRIES; j++) {
            DirectoryEntry tmp = block_read_dir_entry(partition->data_blocks[block_address], j);
            if (strlen(tmp.filename) == 0) {
                block_write_dir_entry(&partition->data_blocks[block_address], j, dir_entry);
                return true;
            }
        }
    }

    return false;
}

// cria arquivo de a partir de um arquivo real do sistema de arquivos real da maquina
// retorna true se criou o arquivo com sucesso, falso caso contrario
bool partition_create_file(Partition *partition, char *filename, INode *dir_inode) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("error: ");
        return false;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // quantidade de blocos necessarios para armazenar o arquivo
    int n_blocks_file = ceil((double)file_size / (double)BLOCK_SIZE);

    // se o arquivo nao couber nos enderecos diretos do inode, entao precisa de um bloco a mais
    // para armazenar os outros enderecos
    if (n_blocks_file > N_DIRECT_BLOCKS) {
        n_blocks_file++;
    }

    // se nao tiver blocos livres o suficiente ou for maior que o tamanho maximo
    if (n_blocks_file > partition->n_free_blocks || file_size > MAX_FILESIZE) {
        return false;
    }

    int32_t inode_address = partition_find_free_inode(partition);
    if (inode_address == -1) {
        return false;
    }

    if (n_blocks_file > N_DIRECT_BLOCKS) {
        char buffer[BLOCK_SIZE];
        uint32_t block_address = -1;

        // preenche os blocos diretos
        for (int j = 0; j < N_DIRECT_BLOCKS; j++) {
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = partition_find_free_block(partition);
            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            partition->inodes[inode_address].direct_blocks[j] = block_address;
            partition->free_blocks_bitmap[block_address] = 1;
        }


        block_address = partition_find_free_block(partition);
        partition->inodes[inode_address].indirect_block = block_address;
        partition->free_blocks_bitmap[block_address] = 1;


        // preenche os blocos indiretos
        int remaining_blocks = n_blocks_file - 1 - N_DIRECT_BLOCKS;
        for (int j = 0; j < remaining_blocks; j++) {
            // se for o ultimo bloco, zera o buffer para garantir escrita correta
            if (j == remaining_blocks-1) {
                memset(buffer, 0, BLOCK_SIZE);
            }
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = partition_find_free_block(partition);
            partition->free_blocks_bitmap[block_address] = 1;

            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            // escreve o endereco no bloco indireto
            block_write_address(&partition->data_blocks[partition->inodes[inode_address].indirect_block], j, block_address);
        }

    }

    // se couber nos enderecos diretos
    else {
        char buffer[BLOCK_SIZE];
        uint32_t block_address = -1;
        for (int j = 0; j < n_blocks_file; j++) {
            if (j == n_blocks_file-1) {
                memset(buffer, 0, BLOCK_SIZE);
            }
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = partition_find_free_block(partition);
            partition->free_blocks_bitmap[block_address] = 1;

            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            partition->inodes[inode_address].direct_blocks[j] = block_address;
        }
    }


    partition->n_free_blocks -= n_blocks_file;

    DirectoryEntry dir_entry;
    dir_entry_set_values(&dir_entry, filename, inode_address);
    partition_insert_dir_entry(partition, dir_inode, dir_entry);

    strncpy(partition->inodes[inode_address].filename, filename, MAX_FILENAME_SIZE);
    partition->inodes[inode_address].size = file_size;
    partition->inodes[inode_address].is_directory = false;

    // TODO
    // partition->inodes[inode_address].created_at =
    // partition->inodes[inode_address].last_accessed_at =
    // partition->inodes[inode_address].modified_at =
    // partition->inodes[inode_address].permissions =


    return true;
}

// void partition_read_file(Partition partition, char *filename) {
//     for (int i = 0; i < N_INODES; i++) {
//         if (strcmp(partition.inodes[i].filename, filename) == 0) {

//         }
//     }
// }
