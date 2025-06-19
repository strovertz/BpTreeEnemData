#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include "base.h"
#include "lista.h"
#include "bst.h"
#include <locale.h>

#define MAX_LINE_LENGTH 8192
#define LINES 3933956
void imprimir_linha_do_offset(FILE *arquivo, long offset);

void buscar_por_inscricao(FILE *filePtr, bptree_node *root) {
    unsigned int inscricao;
    printf("Digite o número de inscrição: ");
    scanf("%u", &inscricao);

    bptree_node *node;
    if (bptree_search(root, inscricao, &node)) {
        int i;
        for (i = 0; i < node->nkeys; i++) {
            if (node->keys[i] == inscricao) {
                long offset = node->offsets[i];
                printf("Inscrição encontrada. Offset: %ld\n", offset);
                imprimir_linha_do_offset(filePtr, offset);
                return;
            }
        }
    } else {
        printf("Inscrição não encontrada.\n");
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


int main(){

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
    Estados *estados = NULL;

    /*
    estados = inserirEstado(estados, "SP");
    estados = inserirEstado(estados, "SC");
    estados = inserirEstado(estados, "MG");
    estados = inserirEstado(estados, "RS");

    inserirCidade(estados, "RS", "Santa Maria", 59974461);
    inserirCidade(estados, "RS", "CAPAO DO CIPO", 59589782);
    inserirCidade(estados, "RS", "CACHOEIRA DO SUL", 59250000);
    inserirCidade(estados, "RS", "CACHOEIRA DO SUL", 59254500);
    inserirCidade(estados, "SP", "Sao Paulo", 59060000);
    inserirCidade(estados, "SP", "Sao Lourenço do Sul", 59300000);
    inserirCidade(estados, "SP", "Sao Gabriel", 59260000);
    inserirCidade(estados, "MG", "Sao Francisco de Paula", 59280000);
    inserirCidade(estados, "SC", "Sao Borja", 59190000);
    inserirCidade(estados, "MG", "Sao Jorge", 59440000);
    inserirCidade(estados, "SC", "Sao Bento do Sul", 59160000);
    inserirCidade(estados, "SC", "Sao Carlos", 59160000);
    inserirCidade(estados, "MG", "Sao Cristovao", 59160000);
    imprimirArvore(estados);
    */

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
        if (nota >= 900) {
            eh_mil = true;
            printf("Redação nota >= 900!\n");
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
        insert_into_leaf(root, inscricao, offset);

        if(i==10) break;
    }

    /* MENU DE BUSCA BONITINHO!(OU QUASE;;;)*/
    menu_busca(filePtr, estados, root, redacoes);
    /*
    printf("Numero de linhas: %d\n", i);
    printf("Imprimindo Redacoes Nota >= 900");
    imprimir_lista(redacoes);
    printf("Imprimindo Arvore de Estados/Cidades + Matriculas");
    imprimirArvore(estados);
    printf("digita o offset que deseja buscar:\n");
    long offset_seek;
    scanf("%ld", &offset_seek);
    inscricao_t insc_busca;
    imprimir_linha_do_offset(filePtr, offset_seek);
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
    }*/
    liberarBPTree(root);
    liberar_lista(redacoes);
    liberarArvore(estados);
    free(root);
    fclose(filePtr);

    return 0;
}
