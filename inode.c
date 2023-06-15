#include "inode.h"
#include "directory_entry.h"

#include <string.h>

void inode_init(INode *inode) {
    strcpy(inode->filename, "");
    inode->size = 0;
    // -1 para indicar invalido
    inode->permissions = -1;
    inode->is_directory = false;
    inode->created_at = -1;
    inode->modified_at = -1;
    inode->last_accessed_at = -1;
    for (int i = 0; i < N_INODE_BLOCK_ADDRESSES; i++) {
        inode->block_addresses[i] = -1;
    }
}
