CC = gcc
MAIN_PROGRAM_NAME = main
CFLAGS = -Wall -Wextra -pedantic -g

BLOCK_SIZE_KB ?= 4 # tamanho do bloco em KB, 4 por padrao
PARTITION_SIZE_MB ?= 10 # tamanho da particao em MB, 10 por padrao
BLOCK_SIZE_BYTES = $$(($(BLOCK_SIZE_KB) * 1024)) # tamanho do bloco em bytes
PARTITION_SIZE_BYTES = $$(($(PARTITION_SIZE_MB) * 1024 * 1024)) # tamanho da particao em bytes
N_BLOCKS = $$(($(PARTITION_SIZE_BYTES) / $(BLOCK_SIZE_BYTES))) # numero total de blocos
N_INODES = $$(($(N_BLOCKS) / 20))	# numero de blocos para inodes, 5% do total de blocos neste caso
N_DATA_BLOCKS = $$(($(N_BLOCKS) - $(N_INODES)))  # numero de blocos para dados, considerando um inode ocupa um bloco

# Generate C macros based on the calculated values
MACROS = -DBLOCK_SIZE_KB=$(BLOCK_SIZE_KB) \
         -DPARTITION_SIZE_MB=$(PARTITION_SIZE_MB) \
         -DBLOCK_SIZE_BYTES=$(BLOCK_SIZE_BYTES) \
         -DPARTITION_SIZE_BYTES=$(PARTITION_SIZE_BYTES) \
         -DN_BLOCKS=$(N_BLOCKS) \
         -DN_INODES=$(N_INODES) \
         -DN_DATA_BLOCKS=$(N_DATA_BLOCKS)

# Find all source files in the directory
SRCS := $(wildcard *.c)

# Generate a list of object files from source files
OBJS := $(SRCS:.c=.o)

# Compilation rule for object files
%.o: %.c
	gcc -c $< -o $@ $(MACROS)

# Compilation rule for the final executable
$(MAIN_PROGRAM_NAME): clean $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN_PROGRAM_NAME)

run: $(MAIN_PROGRAM_NAME)
	./$<

# Clean rule
clean:
	rm -f $(OBJS) $(MAIN_PROGRAM_NAME)
