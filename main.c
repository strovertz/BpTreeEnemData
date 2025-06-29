#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "./files/trees/base.h"
#include "./files/lists/lista.h"
#include "./files/trees/bst.h"
#include <locale.h>
#include "./files/utils/utils.h"

#define MAX_LINE_LENGTH 8192

int main(){

    setlocale(LC_ALL, "");

    char line[MAX_LINE_LENGTH];

    FILE *filePtr = fopen("arquivo.csv", "r");

    if (filePtr == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    inscricao_t inscricao;
    bptree_node *root = create_node(true);
    Estados *estados = NULL;

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
        if (i == 0) {i++; continue;}
        i++;
        char buffer[MAX_LINE_LENGTH];

        strcpy(buffer, line);

        char *insc_field = getfield(buffer, 0);
        fflush(stdout);

        if (insc_field == NULL) {
            fprintf(stderr, "Linha %d inválida (sem inscrição)\n", i);
            continue;
        }

        char *endPtr;
        inscricao = (strtoull(insc_field, &endPtr, 10) % 1000000000);
        bool eh_mil = false;

        /*char buffer2[MAX_LINE_LENGTH];*/

        strcpy(buffer, line);

        char *nota_str = getfield(buffer, 50);
        int nota = parse_nota_redacao(nota_str);

        printf("Nota Redacao (tratada): %d\n", nota);

        /*vamo testar com poucos dados, então finge que +900 = 1000 kkkk*/
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

        /* Trabalha com os dados dos estados e das cidades */

        strcpy(buffer, line);
        estados = insere_cidades(estados, buffer, inscricao);
        strcpy(buffer, line);
        bptree_node *leaf = NULL;
        bptree_search(root, inscricao, &leaf);
        insert_into_leaf(&root, leaf ? leaf : root, inscricao, offset);

        if(i==100000) break;
    }

    menu_busca(filePtr, estados, root, redacoes);

    liberarBPTree(root);
    liberar_lista(redacoes);
    liberarArvore(estados);
    fclose(filePtr);

    return 0;
}
