#include "inode.h"
#include "directory_entry.h"

#include <string.h>

void inode_init(INode *inode) {
    strncpy(inode->filename, "", MAX_FILENAME_SIZE);
    // -1 para indicar invalido
    inode->permissions = -1;
    inode->is_directory = -1;
    inode->created_at = -1;
    inode->modified_at = -1;
    inode->last_accessed_at = -1;
    for (int i = 0; i < MAX_BLOCK_POINTERS; i++) {
        inode->direct_blocks[i] = -1;
    }
    inode->indirect_block = -1;
}
