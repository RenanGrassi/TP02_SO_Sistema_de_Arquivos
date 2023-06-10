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

// retorna o endereco do bloco livre, e retorna -1 se nao achou um bloco livre
static int32_t partition_find_free_block(Partition *partition) {
    for (int32_t i = 0; i < N_DATA_BLOCKS; i++) {
        if (partition->free_blocks_bitmap[i] == 0) {
            return i;
        }
    }
    return -1;
}

// cria arquivo de a partir de um arquivo real do sistema de arquivos real da maquina
// retorna true se criou o arquivo com sucesso, falso caso contrario
bool partition_create_file(Partition *partition, char *filename) {
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

    for (int i = 0; i < N_INODES; i++) {
        if (strlen(partition->inodes[i].filename) == 0) {
            if (n_blocks_file > N_DIRECT_BLOCKS) {
                char buffer[BLOCK_SIZE];
                uint32_t address = -1;

                // preenche os blocos diretos
                for (int j = 0; j < N_DIRECT_BLOCKS; j++) {
                    fread(buffer, 1, BLOCK_SIZE, file);

                    address = partition_find_free_block(partition);
                    // copiando dados para o arquivo simulado
                    memcpy(partition->data_blocks[address].data, buffer, BLOCK_SIZE);

                    partition->inodes[i].direct_blocks[j] = address;
                }


                address = partition_find_free_block(partition);
                partition->inodes[i].indirect_block = address;

                // preenche os blocos indiretos
                for (int j = 0; j < n_blocks_file-1; j++) {
                    fread(buffer, 1, BLOCK_SIZE, file);

                    address = partition_find_free_block(partition);
                    // copiando dados para o arquivo simulado
                    memcpy(partition->data_blocks[address].data, buffer, BLOCK_SIZE);

                    // escreve o endereco no bloco indireto
                    block_write_address(&partition->data_blocks[partition->inodes[i].indirect_block], j, address);
                }

            }

            // se couber nos enderecos diretos
            else {
                for (int j = 0; j < n_blocks_file; j++) {
                    char buffer[BLOCK_SIZE];

                    fread(buffer, 1, BLOCK_SIZE, file);

                    uint32_t address = partition_find_free_block(partition);
                    // copiando dados para o arquivo simulado
                    memcpy(partition->data_blocks[address].data, buffer, BLOCK_SIZE);

                    partition->inodes[i].direct_blocks[j] = address;
                }
            }


            partition->n_free_blocks -= n_blocks_file;

            strncpy(partition->inodes[i].filename, filename, MAX_FILENAME_SIZE);
            partition->inodes[i].size = file_size;
            partition->inodes[i].is_directory = false;

            // TODO
            // partition->inodes[i].created_at =
            // partition->inodes[i].last_accessed_at =
            // partition->inodes[i].modified_at =
            // partition->inodes[i].permissions =

            break;
        }

    }
    return true;
}
