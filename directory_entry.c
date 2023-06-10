#include "directory_entry.h"
#include <string.h>

void dir_entry_init(DirectoryEntry *dir_entry, char *filename, uint32_t inode_address) {
    strncpy(dir_entry->filename, filename, MAX_FILENAME_SIZE);
    dir_entry->inode_address = inode_address;
}
