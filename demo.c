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

#define MAX_LINE_LENGTH 1024
#define LINES 3933956


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

int main(){

    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");

    char line[MAX_LINE_LENGTH];

    FILE *filePtr = fopen("arquivo.csv", "r");

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
        offset = 0; /*ftell(filePtr);*/
        if (fgets(line, MAX_LINE_LENGTH, filePtr) == NULL) break;

        i++;
        char buffer[MAX_LINE_LENGTH];
        strcpy(buffer, line);
        if(i==427 || i==428 || i==429){
            printf("Linha %d: %s", i,line);
        }
        char *insc_field = getfield(buffer, 0);
        fflush(stdout);
        if (insc_field == NULL) {
            fprintf(stderr, "Linha %d inválida (sem inscrição)\n", i);
            continue;
        }
        char *endPtr;
        inscricao = (strtoull(insc_field, &endPtr, 10) % 1000000000);
        bool eh_mil = false;

        char buffer2[MAX_LINE_LENGTH];
        strcpy(buffer2, line);
        /* corrigido para o índice correto da nota de redação: 51 */
        char *nota_str = getfield(buffer2, 50);
        int nota = parse_nota_redacao(nota_str);

        printf("Nota Redacao (tratada): %d\n", nota);

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
       printf("Inscricao: %u, linha %d, offset: %ld\n", inscricao, i, offset);

        insert_into_leaf(root, inscricao, offset);
        insc_field = NULL;
        if(i==2000) break;
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
