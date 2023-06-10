#include <stdio.h>

#include "block.h"
#include "partition.h"

int main(void) {
    Block block;
    block_init(&block);

    block_write_address(&block, 0, 34567);
    block_write_address(&block, 1, 99999);

    for (int i = 0; i < 2; i++) {
        int32_t address = block_read_address(block, i);
        printf("%d\n", address);
    }

    return 0;
}
