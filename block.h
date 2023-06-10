#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

typedef struct {
    // char é 1 byte, logo um vetor de char com tamanho de um bloco vai ocupar exatamente um bloco
    char data[BLOCK_SIZE_BYTES];
} Block;

#endif
