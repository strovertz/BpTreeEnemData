#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "base.h"

#define MAX_LINE_LENGTH 1024
#define LINES 3933956

typedef uint32_t inscricao_t;


char* getfield(char *line, int column) {
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

size_t encode_uleb128 (uint64_t x) {
    unsigned char buf[10];
    size_t bytes = 0;
    do {
        buf[bytes] = x & 0x7fU;
        if (x >>= 7) buf[bytes] |= 0x80U;
        ++bytes;
    } while (x);
    return bytes;
}

/*
size_t decode_uleb128 (uint64_t *x) {
    unsigned char buf;
    size_t bytes = 0;
    while (fread(&buf, 1, 1, in)) {
        if (bytes == 0) *x = 0;
        *x |= (buf & 0x7fULL) << (7 * bytes++);
        if (!(buf & 0x80U)) break;
    }
    return !!bytes;
}
*/

int main(){

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

    int i=0;

    long offset = 0;

    while (true) {
        offset = ftell(filePtr);
        if(fgets(line, MAX_LINE_LENGTH, filePtr) == NULL) break;

        char *endPtr;
        inscricao = (strtoull(getfield(line, 0), &endPtr, 10) % 1000000000);
        printf("inscricao: %u\n", inscricao);
        printf("offset: %ld\n", offset);
        insert_into_leaf(root, inscricao, offset);
        if(++i == 5) break;
    }

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


/*if (field == 0)
        {
            char *endPtr;
            inscricao = (strtoull(getfield(line, 0), &endPtr, 10) % 1000000000);
            printf("%u\n", inscricao);
        } else {
            char *prtStr = getfield(line, field);
            printf("%s\n", prtStr);
        }

        if(inscricao != 0) printf("%llu\n", inscricao);*/
        /*char *strPtr = strtok(line, ";");*/
