#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Inclua o arquivo "PARTICAO_H" aqui
#include "directory_entry.h"
#include "partition.h"

// Sequências de escape de cor ANSI
#define COR_RESET     "\033[0m"
#define COR_VERMELHO  "\033[31m"
#define COR_VERDE     "\033[32m"
#define COR_AMARELO   "\033[33m"
#define COR_AZUL      "\033[34m"
#define COR_MAGENTA   "\033[35m"
#define COR_CYAN      "\033[36m"

// Protótipos das funções
void limpar_tela(void);
void imprimir_menu(void);
void executar_funcao(int escolha, Partition *particao);
void executar_comando(Partition *particao, const char *comando);

int main(void) {
    // Crie uma instância da estrutura Partition
    Partition *particao = malloc(sizeof(Partition));
    partition_init(particao);

    int escolha;
    char dest[MAX_FILENAME_SIZE];
    char nome_arquivo[MAX_FILENAME_SIZE];
    FILE *arquivo;

    do {
        imprimir_menu();

        printf(COR_AMARELO "\nPor favor, escolha uma opção: " COR_RESET);
        scanf("%d", &escolha);
        getchar(); // Consuma o caractere de nova linha


        switch (escolha) {
            case 1:
                printf("Digite o caminho do arquivo real: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                printf("Digite o caminho de destino: ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = '\0';
                if (partition_create_file(particao, dest, nome_arquivo)) {
                    printf(COR_VERDE "Arquivo criado com sucesso.\n" COR_RESET);
                } else {
                    printf(COR_VERMELHO "Falha ao criar o arquivo.\n" COR_RESET);
                }
                break;
            case 2:
                printf("Digite o caminho do arquivo: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                partition_read_file(particao, nome_arquivo);
                break;
            case 3:
                printf("Digite o caminho do diretório: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                if (partition_create_dir(particao, nome_arquivo)) {
                    printf(COR_VERDE "Diretório criado com sucesso.\n" COR_RESET);
                } else {
                    printf(COR_VERMELHO "Falha ao criar o diretório.\n" COR_RESET);
                }
                break;
            case 4:
                printf("Digite o caminho do diretório: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                printf("Digite o novo nome: ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = '\0'; // Remova o caractere de nova linha
                if (partition_rename(particao, nome_arquivo, dest)) {
                    printf(COR_VERDE "Diretório renomeado com sucesso.\n" COR_RESET);
                } else {
                    printf(COR_VERMELHO "Falha ao renomear o diretório.\n" COR_RESET);
                }
                break;
            case 5:
                printf("Digite o caminho do arquivo ou diretório: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                if (partition_delete(particao, nome_arquivo)) {
                    printf(COR_VERDE "Arquivo ou diretório excluído com sucesso.\n" COR_RESET);
                } else {
                    printf(COR_VERMELHO "Falha ao excluir o arquivo ou diretório.\n" COR_RESET);
                }
                break;
            case 6:
                printf("Digite o caminho do diretório: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                partition_list(particao, nome_arquivo);
                break;
            case 7:
                printf("Digite o caminho de origem: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; // Remova o caractere de nova linha
                printf("Digite o caminho de destino: ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = '\0'; // Remova o caractere de nova linha
                if (partition_move(particao, nome_arquivo, dest)) {
                    printf(COR_VERDE "Arquivo ou diretório movido com sucesso.\n" COR_RESET);
                } else {
                    printf(COR_VERMELHO "Falha ao mover o arquivo ou diretório.\n" COR_RESET);
                }
                break;
            case 8:
                printf("Digite o caminho do arquivo: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
                arquivo = fopen(nome_arquivo, "r");
                if (arquivo == NULL) {
                    printf(COR_VERMELHO "Falha ao abrir o arquivo.\n" COR_RESET);
                } else {
                    char linha[200];
                    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
                        linha[strcspn(linha, "\n")] = '\0';
                        executar_comando(particao, linha);
                    }
                    fclose(arquivo);
                }
                break;
            case 9:
                limpar_tela();

                break;
            case 0:
                printf(COR_CYAN "Saindo...\n" COR_RESET);
                break;
            default:
                printf(COR_VERMELHO "Escolha inválida. Por favor, tente novamente.\n" COR_RESET);
        }

        printf("\n");
    } while (escolha != 0);

    free(particao); // Libere a memória alocada para particao

    return 0;
}



void imprimir_menu(void) {
    printf(COR_CYAN "************ %sMenu%s ************\n" COR_RESET, COR_AZUL, COR_CYAN);
    printf(COR_AZUL "1. %sCriar arquivo%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"2. %sLer arquivo%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"3. %sCriar diretório%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"4. %sRenomear diretório%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"5. %sExcluir arquivo ou diretório%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"6. %sListar arquivos em diretório%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"7. %sMover arquivo ou diretório%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"8. %sExecutar comandos de arquivo%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"9. %sLimpar tela%s\n", COR_VERDE, COR_RESET);
    printf(COR_AZUL"0. %sSair%s\n", COR_VERMELHO, COR_RESET);
    printf(COR_CYAN"******************************\n" COR_RESET);
}

void limpar_tela(void) {
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}
void executar_comando(Partition *particao, const char *comando) {
    char copia_comando[200];
    strcpy(copia_comando, comando);

    char token[100];
    char arg1[100];
    char arg2[100];

    if (sscanf(copia_comando, "%s %s %s", token, arg1, arg2) < 1) {
        printf(COR_VERMELHO "Comando inválido.\n" COR_RESET);
        return;
    }

    if (arg1[0] != '\0')
        arg1[strcspn(arg1, "\n")] = '\0';
    if (arg2[0] != '\0')
        arg2[strcspn(arg2, "\n")] = '\0';

    if (strcmp(token, "create_file") == 0) {
        if (arg1[0] == '\0') {
            printf(COR_VERMELHO "Argumento ausente para create_file.\n" COR_RESET);
            return;
        }
        if (partition_create_file(particao, arg1, arg2)) {
            printf(COR_VERDE "Arquivo criado com sucesso.\n" COR_RESET);
        } else {
            printf(COR_VERMELHO "Falha ao criar o arquivo.\n" COR_RESET);
        }
    } else if (strcmp(token, "read_file") == 0) {
        if (arg1[0] == '\0') {
            printf(COR_VERMELHO "Argumento ausente para read_file.\n" COR_RESET);
            return;
        }
        partition_read_file(particao, arg1);
    } else if (strcmp(token, "create_dir") == 0) {
        if (arg1[0] == '\0') {
            printf(COR_VERMELHO "Argumento ausente para create_dir.\n" COR_RESET);
            return;
        }
        if (partition_create_dir(particao, arg1)) {
            printf(COR_VERDE "Diretório criado com sucesso.\n" COR_RESET);
        } else {
            printf(COR_VERMELHO "Falha ao criar o diretório.\n" COR_RESET);
        }
    } else if (strcmp(token, "rename") == 0) {
        if (arg1[0] == '\0' || arg2[0] == '\0') {
            printf(COR_VERMELHO "Argumento(s) ausente(s) para rename.\n" COR_RESET);
            return;
        }
        if (partition_rename(particao, arg1, arg2)) {
            printf(COR_VERDE "Diretório renomeado com sucesso.\n" COR_RESET);
        } else {
            printf(COR_VERMELHO "Falha ao renomear o diretório.\n" COR_RESET);
        }
    } else if (strcmp(token, "delete") == 0) {
        if (arg1[0] == '\0') {
            printf(COR_VERMELHO "Argumento ausente para delete.\n" COR_RESET);
            return;
        }
        if (partition_delete(particao, arg1)) {
            printf(COR_VERDE "Arquivo ou diretório excluído com sucesso.\n" COR_RESET);
        } else {
            printf(COR_VERMELHO "Falha ao excluir o arquivo ou diretório.\n" COR_RESET);
        }
    } else if (strcmp(token, "list") == 0) {
        if (arg1[0] == '\0') {
            printf(COR_VERMELHO "Argumento ausente para list.\n" COR_RESET);
            return;
        }
        partition_list(particao, arg1);
    } else if (strcmp(token, "move") == 0) {
        if (arg1[0] == '\0' || arg2[0] == '\0') {
            printf(COR_VERMELHO "Argumento(s) ausente(s) para move.\n" COR_RESET);
            return;
        }
        if (partition_move(particao, arg1, arg2)) {
            printf(COR_VERDE "Arquivo ou diretório movido com sucesso.\n" COR_RESET);
        } else {
            printf(COR_VERMELHO "Falha ao mover o arquivo ou diretório.\n" COR_RESET);
        }
    } else {
        printf(COR_VERMELHO "Comando inválido: %s\n" COR_RESET, token);
    }
}
