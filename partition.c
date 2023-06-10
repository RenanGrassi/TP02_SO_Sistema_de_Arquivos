#include "partition.h"
#include "block.h"
#include "inode.h"

void partition_init(Partition *partition) {
    for (int i = 0; i < N_DATA_BLOCKS; i++) {
        block_init(&partition->data_blocks[i]);
    }
    for (int i = 0; i < N_INODES; i++) {
        inode_init(&partition->inodes[i]);
    }
    // todos os blocos de dados sao livres inicialmente
    for (int i = 0; i < N_DATA_BLOCKS; i++) {
        partition->free_blocks_bitmap[i] = 0;
    }
}
