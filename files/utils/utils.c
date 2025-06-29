#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "../trees/base.h"
#include "../lists/lista.h"
#include "../trees/bst.h"
#include "utils.h"
#include <locale.h>

#define MAX_LINE_LENGTH 8192

void buscar_por_inscricao(FILE *fp, bptree_node *root) {
    uint64_t chave;

    printf("Digite o número de inscrição DESCONSIDERANDO os 4 primeiros digitos (2100): ");

    scanf("%lu", &chave);
    inscricao_t chave2;
    printf("Chave digitada: %lu\n", chave);
    chave2 = chave % 1000000000;
    printf("Chave reduzida: %u\n", chave2);
    bptree_node *found_node = NULL;
    int idx = bptree_search(root, chave2, &found_node);

    if (found_node && idx >= 0) {
        long offset = found_node->offsets[idx];
        printf("Inscrição encontrada! Offset: %ld\n", offset);
        fseek(fp, offset, SEEK_SET);

        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), fp)) {
            printf("Linha completa: %s\n", buffer);
        } else {
            printf("Erro ao ler a linha no arquivo.\n");
        }
    } else {
        printf("Inscrição %u não encontrada.\n", chave2);
    }
}


void buscar_por_cidade_estado(Estados *estados) {
    char uf[3], cidade[128];
    printf("Digite a sigla do estado (UF): ");
    scanf("%2s", uf);
    getchar();

    printf("Digite o nome da cidade: ");
    fgets(cidade, sizeof(cidade), stdin);
    cidade[strcspn(cidade, "\n")] = '\0';

    Estados *estado = buscarEstado(estados, uf);
    if (!estado) {
        printf("Estado não encontrado.\n");
        return;
    }

    Cidade *cid = buscarCidade(estado->cidades, estado->num_cidades, cidade);
    if (!cid || cid->num_inscricoes == 0) {
        printf("Cidade não encontrada ou sem inscrições.\n");
        return;
    }

    printf("Inscrições para %s - %s:\n", cidade, uf);
    int i;
    for ( i = 0; i < cid->num_inscricoes; i++) {
        printf("- %u\n", cid->inscricoes[i]);
    }
}

void listar_melhores_redacoes(Redacoes *redacoes) {
    if (redacoes == NULL || redacoes->tamanho == 0) {
        printf("Nenhuma redação com nota >= 900 encontrada.\n");
        return;
    }

    printf("Inscrições com nota >= 900 na redação:\n");
    imprimir_lista(redacoes);
}



void menu_busca(FILE *filePtr, Estados *estados, bptree_node *root, Redacoes *redacoes) {
    int opcao;
    do {
        printf("\n--- MENU DE CONSULTA ---\n");
        printf("1 - Buscar por número de inscrição\n");
        printf("2 - Listar inscrições por município + UF\n");
        printf("3 - Listar inscrições com maior nota na redação\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                buscar_por_inscricao(filePtr, root);
                break;
            case 2:
                buscar_por_cidade_estado(estados);
                break;
            case 3:
                listar_melhores_redacoes(redacoes);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}


bool is_numeric(const char *str) {
    if (str == NULL || *str == '\0') return false;
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return false;
    }
    return true;
}

int parse_nota_redacao(const char *nota_str) {
    if (nota_str == NULL || strlen(nota_str) == 0) {
        return 0;
    }
    int i;
    for (i = 0; nota_str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)nota_str[i])) {
            return 0;
        }
    }

    return atoi(nota_str);
}

char* getfield(const char* line, int column) {
    static char buffer[1024];
    int current_col = 0;
    const char* start = line;
    const char* ptr = line;

    while (*ptr && current_col <= column) {
        if (*ptr == ';' || *ptr == '\n') {
            if (current_col == column) {
                size_t len = ptr - start;
                if (len >= sizeof(buffer)) len = sizeof(buffer) - 1;
                strncpy(buffer, start, len);
                buffer[len] = '\0';
                return buffer;
            }
            current_col++;
            ptr++;
            start = ptr;
        } else {
            ptr++;
        }
    }

    if (current_col == column && *start != '\0') {
        strncpy(buffer, start, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        return buffer;
    }

    return NULL;
}

void imprimir_linha_do_offset(FILE *arquivo, long offset) {
    if (fseek(arquivo, offset, SEEK_SET) != 0) {
        perror("Erro ao mover para o offset");
        return;
    }

    int ch;
    if (offset != 0) {
        while (offset > 0) {
            fseek(arquivo, --offset, SEEK_SET);
            ch = fgetc(arquivo);
            if (ch == '\n') {
                break;
            }
        }
    }

    char linha[4096];
    if (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("Linha no offset: %s", linha);
    } else {
        printf("Erro ao ler linha no offset\n");
    }
}

Estados* insere_cidades(Estados *Estados, const char* line, inscricao_t inscricao) {
    int UF = 22;
    int CITY = 20;
    char buffer[MAX_LINE_LENGTH];
    char buffer2[MAX_LINE_LENGTH];
    char cidade_val[MAX_LINE_LENGTH];
    char estado_val[MAX_LINE_LENGTH];

    strcpy(buffer, line);
    char* cidade = getfield(buffer, CITY);
    strncpy(cidade_val, cidade, MAX_LINE_LENGTH);
    cidade_val[MAX_LINE_LENGTH - 1] = '\0';

    strcpy(buffer2, line);
    char* estado = getfield(buffer2, UF);
    strncpy(estado_val, estado, MAX_LINE_LENGTH);
    estado_val[MAX_LINE_LENGTH - 1] = '\0';

    Estados = inserirEstado(Estados, estado_val);
    inserirCidade(Estados, estado_val, cidade_val, inscricao);
    return Estados;
}

