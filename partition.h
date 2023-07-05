#ifndef PARTICAO_H
#define PARTICAO_H

#include <stddef.h>
#include <stdint.h>

#include "inode.h"
#include "block.h"

typedef struct {
    INode inodes[N_INODES]; // vetor dos inodes
    Block data_blocks[N_DATA_BLOCKS]; // vetor dos blocos de dados
    uint8_t free_blocks_bitmap[N_DATA_BLOCKS]; // mapa de bits para controlar os blocos livres: 0 = livre, 1 = ocupado
    int32_t n_free_blocks; // numero de blocos livres
} Partition;

void partition_init(Partition *partition);
bool partition_create_file(Partition *partition, char *dest_dir, char *filepath);
void partition_read_file(Partition *partition, char *filepath);
bool partition_create_dir(Partition *partition, char *filepath);
bool partition_rename(Partition *partition, char *filepath, char *new_filename);
bool partition_delete(Partition *partition, char *filepath);
void partition_list(Partition *partition, char *dir_path);
bool partition_move(Partition *partition, char *filepath, char *dest_dir);




#endif
