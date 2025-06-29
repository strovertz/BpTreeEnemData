#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "base.h"

Estados* criarEstados(const char* sigla_estado) {
    Estados* novo = (Estados*)malloc(sizeof(Estados));
    if (!novo) return NULL;

    novo->sigla_estado = strdup(sigla_estado);
    int i;
    for (i = 0; i < MAX_CIDADES; i++) {
        novo->cidades[i].nome = NULL;
    }

    novo->num_cidades = 0;

    novo->esquerda = novo->direita = NULL;
    return novo;
}

Estados* inserirEstado(Estados* raiz, const char* sigla_estado) {
    if (raiz == NULL)
        return criarEstados(sigla_estado);

    int cmp = strcmp(sigla_estado, raiz->sigla_estado);
    if (cmp < 0)
        raiz->esquerda = inserirEstado(raiz->esquerda, sigla_estado);
    else if (cmp > 0)
        raiz->direita = inserirEstado(raiz->direita, sigla_estado);

    return raiz;
}

void inserirCidade(Estados* raiz, const char* sigla_estado, const char* nome_cidade, inscricao_t inscricao) {
    if (raiz == NULL) return;

    int cmp = strcmp(sigla_estado, raiz->sigla_estado);
    if (cmp < 0) {
        inserirCidade(raiz->esquerda, sigla_estado, nome_cidade, inscricao);
    } else if (cmp > 0) {
        inserirCidade(raiz->direita, sigla_estado, nome_cidade, inscricao);
    } else {
        int i;
        for (i = 0; i < raiz->num_cidades; i++) {
            if (strcmp(raiz->cidades[i].nome, nome_cidade) == 0) {
                adicionarInscricao(&raiz->cidades[i], inscricao);
                return;
            }
        }

        if (raiz->num_cidades < MAX_CIDADES) {
            Cidade *c = &raiz->cidades[raiz->num_cidades];
            c->nome = strdup(nome_cidade);
            c->inscricoes = NULL;
            c->num_inscricoes = 0;
            c->capacidade = 0;
            adicionarInscricao(c, inscricao);
            raiz->num_cidades++;
        }
    }
}


void adicionarInscricao(Cidade* cidade, inscricao_t inscricao) {
    if (cidade->num_inscricoes == cidade->capacidade) {
        int nova_capacidade = cidade->capacidade == 0 ? 4 : cidade->capacidade * 2;
        inscricao_t *novo_array = realloc(cidade->inscricoes, nova_capacidade * sizeof(inscricao_t));
        if (!novo_array) {
            perror("Falha ao realocar memória para inscrições");
            return;
        }
        cidade->inscricoes = novo_array;
        cidade->capacidade = nova_capacidade;
    }
    cidade->inscricoes[cidade->num_inscricoes++] = inscricao;
}

Estados* buscarEstado(Estados* raiz, const char* sigla_estado) {
    if (raiz == NULL) return NULL;

    int cmp = strcmp(sigla_estado, raiz->sigla_estado);
    if (cmp == 0) {
        return raiz;
    } else if (cmp < 0) {
        return buscarEstado(raiz->esquerda, sigla_estado);
    } else {
        return buscarEstado(raiz->direita, sigla_estado);
    }
}

Cidade* buscarCidade(Cidade* cidades, int num_cidades, const char* nome_cidade) {
    int i;
    for (i = 0; i < num_cidades; i++) {
        if (strcmp(cidades[i].nome, nome_cidade) == 0) {
            return &cidades[i];
        }
    }
    return NULL;
}


void liberarArvore(Estados* raiz) {
    if (raiz == NULL) return;

    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    int i;
    for (i = 0; i < raiz->num_cidades; i++) {
        free(raiz->cidades[i].nome);
        free(raiz->cidades[i].inscricoes);
    }
    free(raiz->sigla_estado);
    free(raiz);
}


void imprimirArvore(Estados* raiz) {
    if (raiz == NULL) return;

    imprimirArvore(raiz->esquerda);

    printf("Estado: %s\n", raiz->sigla_estado);
    printf("  Cidades:\n");
    int i;
    for (i = 0; i < raiz->num_cidades; i++) {
        printf("Inscritos:   - %d\n", raiz->cidades[i].num_inscricoes);
        printf("    - %s (Inscricoes: ", raiz->cidades[i].nome);
        int j;
        for (j = 0; j < raiz->cidades[i].num_inscricoes; j++) {
            printf("%u", raiz->cidades[i].inscricoes[j]);
            if (j < raiz->cidades[i].num_inscricoes - 1) printf(", ");
        }
        printf(")\n");
    }

    imprimirArvore(raiz->direita);
}
