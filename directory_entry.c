#include "directory_entry.h"
#include <string.h>

void dir_entry_set_values(DirectoryEntry *dir_entry, char *filename, uint32_t inode_address) {
    strcpy(dir_entry->filename, filename);
    dir_entry->inode_address = inode_address;
}
