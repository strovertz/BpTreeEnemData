#ifndef BST_H
#define BST_H
#include "base.h"

#define MAX_CIDADES 200
#define MAX_INSCRICAO
typedef struct {
    char *nome;
    inscricao_t *inscricoes;
    int num_inscricoes;
    int capacidade;
} Cidade;

typedef struct Estados {
    char *sigla_estado;
    Cidade cidades[MAX_CIDADES];
    int num_cidades;
    struct Estados *esquerda, *direita;
} Estados;

Estados* inserirEstado(Estados* raiz, const char* sigla_estado);
void inserirCidade(Estados* raiz, const char* sigla_estado, const char* nome_cidade, inscricao_t inscricao);
void adicionarInscricao(Cidade* cidade, inscricao_t inscricao);
Estados* criarNo(const char* sigla_estado);
Estados* buscarEstado(Estados* raiz, const char* sigla_estado);
Cidade* buscarCidade(Cidade* cidades, int num_cidades, const char* nome_cidade);
void liberarArvore(Estados* raiz);
void imprimirArvore(Estados* raiz);

#endif /* BST_H */
