CC = gcc
MAIN_PROGRAM_NAME = main
CFLAGS = -Wall -Wextra -pedantic -g

# MACROS (os valores são definidos em tempo de preprocessamento)
BLOCK_SIZE_KB ?= 4 	# tamanho do bloco em KB, 4 por padrao
PARTITION_SIZE_MB ?= 10 # tamanho da particao em MB, 10 por padrao
BLOCK_ADDRESS_SIZE = 4 # tamanho de endereço de bloco em bytes
BLOCK_SIZE = $$(($(BLOCK_SIZE_KB) * 1024)) # tamanho do bloco em bytes
PARTITION_SIZE = $$(($(PARTITION_SIZE_MB) * 1024 * 1024)) # tamanho da particao em bytes
N_BLOCKS = $$(($(PARTITION_SIZE) / $(BLOCK_SIZE))) # numero total de blocos
N_INODES = $$(($(N_BLOCKS) / 20)) # numero de blocos para inodes, 5% do total de blocos neste caso
N_DATA_BLOCKS = $$(($(N_BLOCKS) - $(N_INODES)))  # numero de blocos para dados, considerando um inode ocupa um bloco
N_BLOCK_ADDRESSES = $$(($(BLOCK_SIZE) / $(BLOCK_ADDRESS_SIZE))) # numero de enderecos de bloco que cabem em um bloco

# Generate C macros based on the calculated values
MACROS = -DBLOCK_SIZE_KB=$(BLOCK_SIZE_KB) \
         -DPARTITION_SIZE_MB=$(PARTITION_SIZE_MB) \
         -DBLOCK_SIZE=$(BLOCK_SIZE) \
         -DPARTITION_SIZE=$(PARTITION_SIZE) \
         -DN_BLOCKS=$(N_BLOCKS) \
         -DN_INODES=$(N_INODES) \
         -DN_DATA_BLOCKS=$(N_DATA_BLOCKS) \
		 -DBLOCK_ADDRESS_SIZE=$(BLOCK_ADDRESS_SIZE) \
		 -DN_BLOCK_ADDRESSES=$(N_BLOCK_ADDRESSES)

# Find all source files in the directory
SRCS := $(wildcard *.c)

# Generate a list of object files from source files
OBJS := $(SRCS:.c=.o)

# Compilation rule for object files
%.o: %.c
	@gcc -c $< -o $@ $(MACROS) $(CFLAGS)

# Compilation rule for the final executable
$(MAIN_PROGRAM_NAME): clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN_PROGRAM_NAME)

run: $(MAIN_PROGRAM_NAME)
	./$<

# Clean rule
clean:
	rm -f $(OBJS) $(MAIN_PROGRAM_NAME)
