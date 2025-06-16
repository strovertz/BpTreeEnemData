#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "base.h"
#include "lista.h"
#include <locale.h>
#include <windows.h>

#define MAX_LINE_LENGTH 8192
#define LINES 3933956


bool is_numeric(const char *str) {
    if (str == NULL || *str == '\0') return false;
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return false;
    }
    return true;
}

int parse_nota_redacao(const char *str) {
    if (str == NULL) return 0;

    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') return 0;
    const char *p;
    for (p = str; *p != '\0'; p++) {
        if (!isdigit((unsigned char)*p)) return 0;
    }

    return atoi(str);
}

char* getfield2(char *line, int column) {
    char *token = strtok(line, ";");

    int i = 0;

    while (token != NULL) {
        if (i == column) {
            return token;
        }
        token = strtok(NULL, ";");
        i++;
    }
    return 0;
}

char* getfield(const char* line, int column) {
    static char temp[1024];
    int current_col = 0;
    const char* start = line;
    const char* ptr = line;

    while (*ptr && current_col <= column) {
        if (*ptr == ';' || *ptr == '\n') {
            if (current_col == column) {
                size_t len = ptr - start;
                if (len >= sizeof(temp)) len = sizeof(temp) - 1;
                strncpy(temp, start, len);
                temp[len] = '\0';
                return temp;
            }
            current_col++;
            ptr++;
            start = ptr;
        } else {
            ptr++;
        }
    }

    if (current_col == column && *start != '\0') {
        strncpy(temp, start, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        return temp;
    }

    return NULL;
}

int main(){

    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");

    char line[MAX_LINE_LENGTH];

    FILE *filePtr = fopen("MICRODADOS_ENEM_2023.CSV", "r");

    if (filePtr == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    /*Ler header */
    /*fgets(line, sizeof(line), filePtr);
    line[strlen(line) - 1] = '\0';
    printf("Header: %s", line);
    int field = 0;*/

    inscricao_t inscricao;
    bptree_node *root = create_node(true);
    bptree_node *node = NULL;

    Redacoes *redacoes = NULL;
    inicializar_lista(&redacoes, 2);
    if (redacoes == NULL) {
        fprintf(stderr, "Erro ao inicializar lista de redacoes Nota 1000.\n");
        fclose(filePtr);
        return EXIT_FAILURE;
    }

    if (root == NULL) {
        fprintf(stderr, "Erro ao criar nó raiz.\n");
        fclose(filePtr);
        return EXIT_FAILURE;
    }

    int i=0;

    long offset = 0;

    /* Ler linhas do arquivo,
    envia a struct base_data_t
    para getfield com o campo que desejar
    (tem q ser o valor em numeros da ; pós campo desejado)
    então retorna o campo desejado*/
    while (true) {
        offset = ftell(filePtr);
        if (fgets(line, MAX_LINE_LENGTH, filePtr) == NULL) break;
        line[strcspn(line, "\r\n")] = 0;
        i++;

        char buffer[MAX_LINE_LENGTH], buffer2[MAX_LINE_LENGTH];
        strncpy(buffer, line, MAX_LINE_LENGTH);
        strncpy(buffer2, line, MAX_LINE_LENGTH);

        char *insc_field = getfield(buffer, 0);
        if (insc_field == NULL) {
            fprintf(stderr, "Linha %d inválida (sem inscrição)\n", i);
            continue;
        }
        bool eh_mil = false;
        char *endPtr;
        inscricao = (strtoull(insc_field, &endPtr, 10) % 1000000000);

        strcpy(buffer2, line);
        char *nota_str = getfield(buffer2, 50);
        int nota = 0;
        if (nota_str == NULL) {
            fprintf(stderr, "Linha %d: campo 50 ausente. Linha original: %s", i, line);
        }
        if (nota_str == NULL || strlen(nota_str) == 0 || !isdigit(nota_str[0])) {
            nota = 0;
            printf("Linha %d: nota ausente ou inválida, tratado como 0\n", i);
        } else {
            nota = atoi(nota_str);
            printf("Nota Redacao (tratada): %d\n", nota);
        }

        if (nota == 1000) {
            eh_mil = true;
            printf("Redação nota 1000!\n");
            inserir_no_final(redacoes, (redacao_t){.inscricao = inscricao, .eh_mil = true});
        }

        if (eh_mil) {
            printf("inscricao: %u\n", inscricao);
            printf("offset: %ld\n", offset);
            printf("Nota Redacao: %d\n", nota);
        }

        insert_into_leaf(root, inscricao, offset);
    }

    printf("Numero de linhas: %d\n", i);

    printf("digita a mtricula que deseja buscar:\n");

    inscricao_t insc_busca;

    scanf("%u", &insc_busca);
    if (bptree_search(root, insc_busca, &node)) {
        printf("Chave %u encontrada no node!\n", insc_busca);
        for (i = 0; i < node->nkeys; i++) {
            if (node->keys[i] == insc_busca) {
                printf("Offset da linha no arquivo: %ld\n", node->offsets[i]);
                break;
            }
        }
    } else {
        printf("Chave %u não encontrada.\n", insc_busca);
    }
    fclose(filePtr);

    return 0;
}
