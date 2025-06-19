#ifndef __LISTA_H__
#define __LISTA_H__
#include "../trees/base.h"

typedef struct redacao_t{
    inscricao_t inscricao;
    bool eh_mil;
} redacao_t;

typedef struct {
    redacao_t* dados;
    size_t tamanho;
    size_t capacidade;
} Redacoes;

void inicializar_lista(Redacoes **lista, size_t capacidade_inicial);
void inserir_no_final(Redacoes* lista, redacao_t elemento);
void imprimir_lista(const Redacoes* lista);
void liberar_lista(Redacoes* lista);

#endif
