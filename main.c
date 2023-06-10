#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "directory_entry.h"
#include "partition.h"

int main(void) {
    // -----------------------------TESTES------------------------------
    Block block1;
    block_init(&block1);
    Block block2;
    block_init(&block2);

    block_write_address(&block1, 0, 34567);
    block_write_address(&block1, 1, 99999);

    DirectoryEntry dir_entry1;
    dir_entry_set_values(&dir_entry1, "test1", 123);
    DirectoryEntry dir_entry2;
    dir_entry_set_values(&dir_entry2, "test2", 456);
    block_write_dir_entry(&block2, 0, dir_entry1);
    block_write_dir_entry(&block2, 1, dir_entry2);

    for (int i = 0; i < 2; i++) {
        int32_t address = block_read_address(block1, i);
        printf("%d\n", address);
    }
    for (int i = 0; i < 2; i++) {
        DirectoryEntry dir_entry = block_read_dir_entry(block2, i);
        printf("%s, %d\n", dir_entry.filename, dir_entry.inode_address);
    }

    Partition *partition = malloc(sizeof(Partition));
    partition_init(partition);

    free(partition);
    // -----------------------------------------------------------------

    return 0;
}
