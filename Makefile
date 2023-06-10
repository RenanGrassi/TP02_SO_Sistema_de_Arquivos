CC = gcc
MAIN_PROGRAM_NAME = main
CFLAGS = -Wall -Wextra -pedantic -g

# Default values for BLOCK_SIZE_KB and PARTITION_SIZE_MB
BLOCK_SIZE_KB ?= 4
PARTITION_SIZE_MB ?= 10
BLOCK_SIZE_BYTES = $$(($(BLOCK_SIZE_KB) * 1024))
PARTITION_SIZE_BYTES = $$(($(PARTITION_SIZE_MB) * 1024 * 1024))
N_BLOCKS = $$(($(PARTITION_SIZE_BYTES) / $(BLOCK_SIZE_BYTES)))
N_INODES = $$(($(N_BLOCKS) / 20))	# 5% dos blocos são para inodes
N_DATA_BLOCKS = $$(($(N_BLOCKS) - $(N_INODES)))

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
$(MAIN_PROGRAM_NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN_PROGRAM_NAME)

# Clean rule
clean:
	rm -f $(OBJS) $(MAIN_PROGRAM_NAME)
