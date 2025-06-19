#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "lista.h"

/* Lista dinamica pra armazenar todas as matriculas com redacoes nota mil,
   lista dinamica pra ser mais rapido de buscar na memoria pq fica
   armazenado de forma contigua; ggwp */

void inicializar_lista(Redacoes **lista, size_t capacidade_inicial) {
    *lista = malloc(sizeof(Redacoes));
    (*lista)->dados = malloc(capacidade_inicial * sizeof(redacao_t));
    (*lista)->tamanho = 0;
    (*lista)->capacidade = capacidade_inicial;
}
void inserir_no_final(Redacoes* lista, redacao_t elemento) {
    if (lista->tamanho == lista->capacidade) {
        lista->capacidade *= 2; /**/
        lista->dados = realloc(lista->dados, lista->capacidade * sizeof(redacao_t));
    }
    lista->dados[lista->tamanho++] = elemento;
}

void imprimir_lista(const Redacoes* lista) {
    size_t i;
    for (i = 0; i < lista->tamanho; i++) {
        printf("Inscrição: %u - Nota: %u\n", lista->dados[i].inscricao, lista->dados[i].eh_mil ? 1000 : printf("Erro, nota inválida\n"));
    }
}

void liberar_lista(Redacoes* lista) {
    free(lista->dados);
    lista->dados = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
}
