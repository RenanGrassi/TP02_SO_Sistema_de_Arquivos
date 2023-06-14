#include "partition.h"
#include "block.h"
#include "directory_entry.h"
#include "inode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>

void partition_init(Partition *partition) {
    for (int i = 0; i < N_DATA_BLOCKS; i++) {
        block_init(&partition->data_blocks[i]);
    }
    for (int i = 0; i < N_INODES; i++) {
        inode_init(&partition->inodes[i]);
    }
    // inicializa com zeros
    memset(partition->free_blocks_bitmap, 0, N_DATA_BLOCKS * sizeof(int8_t));
    partition->n_free_blocks = N_DATA_BLOCKS;

    // cria diretorio root "/"
    INode root;
    inode_init(&root);

    // o primeiro bloco é sempre o root: mauricio, corrige se estiver errado :)
    strcpy(root.filename, "/");
    root.size = 0;
    root.is_directory = true;
    root.created_at = time(NULL);
    root.last_accessed_at = root.created_at;
    root.modified_at =  root.created_at; 
    root.permissions = 0777; // 777 em octal

    // o primeiro inode é sempre o root
    partition->inodes[0] = root;
}

// retorna o endereco de um bloco livre, e retorna -1 se nao achou
static int32_t find_free_block(Partition *partition) {
    for (int32_t i = 0; i < N_DATA_BLOCKS; i++) {
        if (partition->free_blocks_bitmap[i] == 0) {
            return i;
        }
    }
    return -1;
}

// retorna o endereco de um inode livre, e retorna -1 se nao achou
static int32_t find_free_inode(Partition *partition) {
    for (int32_t i = 0; i < N_INODES; i++) {
        if (strlen(partition->inodes[i].filename) == 0) {
            return i;
        }
    }
    return -1;
}


// procura filename em um diretorio e retorna o endereco do inode se achar
static int32_t find_filename_in_dir(Partition *partition, INode dir_inode, char *filename) {
    if (dir_inode.is_directory == true) {
        int32_t block_address = -1;
        for (int i = 0; i < N_INODE_BLOCK_ADDRESSES ; i++) {
            block_address = dir_inode.address_blocks[i];

            if (block_address == -1) {
                return -1;
            }

            for (int i = 0; i < N_DIR_ENTRIES; i++) {
                DirectoryEntry dir_entry = block_read_dir_entry(partition->data_blocks[block_address], i);
                if (strcmp(dir_entry.filename, filename) == 0) {
                    return dir_entry.inode_number;
                }
            }

        }
    }

    return -1;
}

// insere uma entrada de diretorio em um inode de diretorio
bool partition_insert_dir_entry(Partition *partition, INode *dir_inode, DirectoryEntry dir_entry) {
    if (!dir_inode->is_directory) {
        return false;
    }


    // procura um bloco
    for (int i = 0; i < N_INODE_BLOCK_ADDRESSES; i++) {
        int32_t block_address = -1;
        block_address = dir_inode->address_blocks[i];
        if (block_address == -1) {
            block_address = find_free_block(partition);
            if (block_address == -1) {
                return false;
            }

            dir_inode->address_blocks[i] = block_address;
            partition->free_blocks_bitmap[block_address] = 1;
            partition->n_free_blocks--;
        }

        // procura um lugar vazio no bloco para escrever a entrada de diretorio
        for (int j = 0; j < N_DIR_ENTRIES; j++) {
            DirectoryEntry tmp = block_read_dir_entry(partition->data_blocks[block_address], j);
            if (strlen(tmp.filename) == 0) {
                block_write_dir_entry(&partition->data_blocks[block_address], j, dir_entry);
                return true;
            }
        }
    }

    return false;
}

// cria arquivo de a partir de um arquivo real do sistema de arquivos real da maquina
// retorna true se criou o arquivo com sucesso, falso caso contrario
bool partition_create_file(Partition *partition, char *filename, INode *dir_inode) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("error");
        fclose(file);
        return false;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // quantidade de blocos necessarios para armazenar o arquivo
    int n_blocks_file = ceil((double)file_size / (double)BLOCK_SIZE);

    // se o arquivo nao couber nos enderecos diretos do inode, entao precisa de um bloco a mais
    // para armazenar os outros enderecos
    if (n_blocks_file > N_INODE_BLOCK_ADDRESSES-1) {
        n_blocks_file++;
    }

    // se nao tiver blocos livres o suficiente ou for maior que o tamanho maximo
    if (n_blocks_file > partition->n_free_blocks || file_size > MAX_FILESIZE) {
        printf("Erro: arquivo não cabe na partição\n");
        fclose(file);
        return false;
    }

    int32_t inode_number = find_free_inode(partition);
    if (inode_number == -1) {
        printf("Erro: lista de inodes cheia, não é possível criar o arquivo\n");
        fclose(file);
        return false;
    }

    // se nao couber nos blocos diretos
    if (n_blocks_file > N_INODE_BLOCK_ADDRESSES-1) {
        char buffer[BLOCK_SIZE];
        uint32_t block_address = -1;

        // preenche os blocos diretos
        for (int j = 0; j < N_INODE_BLOCK_ADDRESSES-1; j++) {
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = find_free_block(partition);
            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            partition->inodes[inode_number].address_blocks[j] = block_address;
            partition->free_blocks_bitmap[block_address] = 1;
        }

        // acha um bloco para o bloco indireto
        block_address = find_free_block(partition);
        partition->inodes[inode_number].address_blocks[N_INODE_BLOCK_ADDRESSES-1] = block_address;
        partition->free_blocks_bitmap[block_address] = 1;


        // preenche os blocos indiretos
        int remaining_blocks = n_blocks_file - N_INODE_BLOCK_ADDRESSES;
        for (int j = 0; j < remaining_blocks; j++) {
            // se for o ultimo bloco, zera o buffer para garantir escrita correta
            if (j == remaining_blocks-1) {
                memset(buffer, 0, BLOCK_SIZE);
            }
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = find_free_block(partition);
            partition->free_blocks_bitmap[block_address] = 1;

            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            // escreve o endereco no bloco indireto
            block_write_address(&partition->data_blocks[partition->inodes[inode_number].address_blocks[N_INODE_BLOCK_ADDRESSES-1]], j, block_address);
        }

        // preenche o resto com -1
        for (int j = remaining_blocks; j < N_BLOCK_ADDRESSES; j++) {
            block_write_address(&partition->data_blocks[partition->inodes[inode_number].address_blocks[N_INODE_BLOCK_ADDRESSES-1]], j, -1);
        }

    }

    // se couber nos enderecos diretos
    else {
        char buffer[BLOCK_SIZE];
        uint32_t block_address = -1;
        for (int j = 0; j < n_blocks_file; j++) {
            if (j == n_blocks_file-1) {
                memset(buffer, 0, BLOCK_SIZE);
            }
            fread(buffer, 1, BLOCK_SIZE, file);

            block_address = find_free_block(partition);
            partition->free_blocks_bitmap[block_address] = 1;

            // copiando dados para o arquivo simulado
            memcpy(partition->data_blocks[block_address].data, buffer, BLOCK_SIZE);

            partition->inodes[inode_number].address_blocks[j] = block_address;
        }
    }


    partition->n_free_blocks -= n_blocks_file;

    // insere arquivo no diretorio
    DirectoryEntry dir_entry;
    dir_entry_set_values(&dir_entry, filename, inode_number);
    partition_insert_dir_entry(partition, dir_inode, dir_entry);

    strcpy(partition->inodes[inode_number].filename, filename);
    partition->inodes[inode_number].size = file_size;
    partition->inodes[inode_number].is_directory = false;

    // data de criacao, ultimo acesso e modificacao
    partition->inodes[inode_number].created_at = time(NULL);
    partition->inodes[inode_number].last_accessed_at = partition->inodes[inode_number].created_at;
    partition->inodes[inode_number].modified_at = partition->inodes[inode_number].created_at;
    partition->inodes[inode_number].permissions = 0777;

    fclose(file);
    return true;
}

// le arquivo e printa
void partition_read_file(Partition *partition, char *filepath) {
    // comeca pelo root(o primeiro inode é o root)
    INode inode = partition->inodes[0];

    char* token = strtok(filepath, "/");
    while (inode.is_directory) {
        int32_t inode_number = find_filename_in_dir(partition, inode, token);
        if (inode_number == -1) {
            printf("Erro: arquivo não encontrado\n");
            return;
        }

        inode = partition->inodes[inode_number];
        token = strtok(NULL, "/");
    }

    int32_t block_address = -1;
    char *buffer = malloc(BLOCK_SIZE+1);
    // blocos diretos
    for (int i = 0; i < N_INODE_BLOCK_ADDRESSES-1; i++) {
        block_address = inode.address_blocks[i];
        if (block_address == -1) {
            printf("\n");
            free(buffer);
            return;
        }

        strncpy(buffer, partition->data_blocks[block_address].data, BLOCK_SIZE);
        buffer[BLOCK_SIZE] = '\0';
        printf("%s", buffer);
    }

    // bloco indireto
    for (int i = 0; i < N_BLOCK_ADDRESSES; i++) {
        block_address = block_read_address(partition->data_blocks[inode.address_blocks[N_INODE_BLOCK_ADDRESSES-1]], i);
        if (block_address == -1) {
            printf("\n");
            free(buffer);
            return;
        }

        strncpy(buffer, partition->data_blocks[block_address].data, BLOCK_SIZE);
        buffer[BLOCK_SIZE] = '\0';
        printf("%s", buffer);
    }

    printf("\n");
    free(buffer);

}

//cria diretorio
bool partition_create_dir(Partition *partition, INode *dir_inode, char *filename){
    // verifica se ja existe um arquivo com esse nome
    int32_t inode_number = find_filename_in_dir(partition, *dir_inode, filename);
    if (inode_number != -1) {
        printf("Erro: arquivo já existe\n");
        return false;
    }

    // acha um inode livre
    inode_number = find_free_inode(partition);
    if (inode_number == -1) {
        printf("Erro: não há inodes livres\n");
        return false;
    }

    // insere arquivo no diretorio
    DirectoryEntry dir_entry;
    dir_entry_set_values(&dir_entry, filename, inode_number);
    partition_insert_dir_entry(partition, dir_inode, dir_entry);

    strcpy(partition->inodes[inode_number].filename, filename);
    partition->inodes[inode_number].size = 0;
    partition->inodes[inode_number].is_directory = true;

    // data de criacao, ultimo acesso e modificacao
    partition->inodes[inode_number].created_at = time(NULL);
    partition->inodes[inode_number].last_accessed_at = partition->inodes[inode_number].created_at;
    partition->inodes[inode_number].modified_at = partition->inodes[inode_number].created_at;
    partition->inodes[inode_number].permissions = 0777;

    return true;
}

//Renomear tanto arquivos quanto diretorios
bool partition_rename(Partition *partition, INode *dir_inode, char *filename, char *new_filename){
    if (dir_inode->is_directory == true) {
        int32_t block_address = -1;
        for (int i = 0; i < N_INODE_BLOCK_ADDRESSES ; i++) {
            block_address = dir_inode->address_blocks[i];

            if (block_address == -1) {
                return false;
            }

            for (int i = 0; i < N_DIR_ENTRIES; i++) {
                DirectoryEntry dir_entry = block_read_dir_entry(partition->data_blocks[block_address], i);
                if (strcmp(dir_entry.filename, filename) == 0) {
                    strcpy(dir_entry.filename, new_filename);
                    block_write_dir_entry(partition->data_blocks[block_address], i, dir_entry);
                    int32_t leo = dir_entry.inode_number;
                    strcpy(partition->inodes[leo].filename, new_filename);
                    return true;
                }
            }
        }
    }

    return false;
}

//Remove arquivos e diretorios
bool partition_delete(Partition *partition, INode *dir_inode, char *filename){
    if (dir_inode->is_directory == true) {
        int32_t block_address = -1;
        for (int i = 0; i < N_INODE_BLOCK_ADDRESSES ; i++) {
            block_address = dir_inode->address_blocks[i];

            if (block_address == -1) {
                return false;
            }

            for (int i = 0; i < N_DIR_ENTRIES; i++) {
                DirectoryEntry dir_entry = block_read_dir_entry(partition->data_blocks[block_address], i);
                if (strcmp(dir_entry.filename, filename) == 0) {
                    int32_t inode_number = dir_entry.inode_number;
                    dir_entry.inode_number = -1;
                    strcpy(dir_entry.filename, "");
                    block_write_dir_entry(partition->data_blocks[block_address], i, dir_entry);
                    strcpy(partition->inodes[inode_number].filename, "");
                    if(partition->inodes[inode_number].is_directory == true){
                        // TODO
                    } else {
                        // blocos diretos
                        for (size_t i = 0; i < N_INODE_BLOCK_ADDRESSES - 1; i++){
                            block_address = partition->inodes[inode_number].address_blocks[i];
                            if (block_address != -1)
                            {
                                partition->free_blocks_bitmap[block_address] = 0;
                                partition->n_free_blocks++;
                                partition->inodes[inode_number].address_blocks[i] = -1;
                            }
                            
                            
                        }
                        int32_t indirect_block_address = partition->inodes[inode_number].address_blocks[N_INODE_BLOCK_ADDRESSES - 1];
                        if (indirect_block_address != -1) {
                            // preenche o bloco indireto com -1
                            for (size_t i = 0; i < N_BLOCK_ADDRESSES; i++)
                            {
                                block_address = block_read_address(partition->data_blocks[indirect_block_address], i);
                                
                                if (block_address != -1)
                                {
                                    partition->free_blocks_bitmap[block_address] = 0;
                                    partition->n_free_blocks++;
                                    block_write_address(&partition->data_blocks[indirect_block_address], i, -1);
                                }
                                
                            }
                            partition->inodes[inode_number].address_blocks[N_INODE_BLOCK_ADDRESSES - 1] = -1;
                            partition->free_blocks_bitmap[indirect_block_address] = 0;
                            partition->n_free_blocks++;
                        }
                        partition->inodes[inode_number].size = 0;
                        partition->inodes[inode_number].permissions = -1;
                        partition->inodes[inode_number].is_directory = false;
                        partition->inodes[inode_number].created_at = -1;
                        partition->inodes[inode_number].modified_at = -1;
                        partition->inodes[inode_number].last_accessed_at = -1;
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

//listar conteúdo do diretório
void partition_list(Partition *partition, INode *dir_inode){    
    if (dir_inode->is_directory == true) {
        int32_t block_address = -1;
        for (int i = 0; i < N_INODE_BLOCK_ADDRESSES ; i++) {
            block_address = dir_inode->address_blocks[i];

            if (block_address == -1) {
                break;
            }
            for (int i = 0; i < N_DIR_ENTRIES; i++) {
                DirectoryEntry dir_entry = block_read_dir_entry(partition->data_blocks[block_address], i);
                if (strcmp(dir_entry.filename, "") != 0) {
                    int32_t inode_number = dir_entry.inode_number;
                    printf("Inode %d/%d", inode_number, N_INODES);
                    printf("");
                    INode inode = partition->inodes[inode_number];
                    if(dir_inode->is_directory == true){
                        printf("Diretório: %s\n", dir_entry.filename);
                    } else {
                        printf("Arquivo: %s\n", dir_entry.filename);
                    }
                    printf("Tamanho: %d\n", inode.size);
                    printf("Permissões: %d\n", inode.permissions);
                    printf("Criado em: %s", ctime(&inode.created_at));
                    printf("Modificado em: %s", ctime(&inode.modified_at));
                    printf("Acessado em: %s", ctime(&inode.last_accessed_at));
                    print("");
                    printf("######################################################");
                }
            }
        }
    }
}

//mover arquivo ou diretório
bool partition_move(Partition *partition, INode *dir_inode, char *filename){
    if (dir_inode->is_directory == true) {
        int32_t block_address = -1;
        for (int i = 0; i < N_INODE_BLOCK_ADDRESSES ; i++) {
            block_address = dir_inode->address_blocks[i];

            if (block_address == -1) {
                return false;
            }

            for (int i = 0; i < N_DIR_ENTRIES; i++) {
                DirectoryEntry dir_entry = block_read_dir_entry(partition->data_blocks[block_address], i);
                if (strcmp(dir_entry.filename, filename) == 0) {
                    return dir_entry.inode_number;
                }
            }

        }
    }
    return -1;
}  