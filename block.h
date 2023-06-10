#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

typedef struct {
    char data[BLOCK_SIZE_BYTES];
} Block;

#endif
