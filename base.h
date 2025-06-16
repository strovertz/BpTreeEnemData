#ifndef __BASE_H__
#define __BASE_H__

#include <stdint.h>

/*NAO ESQUECER DE AJUSTA!!!*/
#define M 128

typedef uint32_t inscricao_t;
typedef uint16_t nota_t;
typedef struct base_data_t {
    inscricao_t inscricao;
    bool eh_mil;
    long offset;
}base_data_t;

typedef struct bptree_node {
    int nkeys;                    /* quantidade atual de chaves no nó */
    uint32_t keys[M - 1];         /* vetor de chaves (matrículas) */
    long offsets[M - 1];          /* vetor de offsets (só usado em folhas) */
    struct bptree_node *children[M]; /* ponteiros para filhos (NULL se folha) */
    struct bptree_node *next;     /* próximo nó folha (usado só em folhas) */
    bool is_leaf;                 /* flag para diferenciar nó folha ou interno */
} bptree_node;

bptree_node* create_node(bool is_leaf);
int bptree_search(bptree_node *node, uint32_t key, bptree_node **found_node);
void insert_into_leaf(bptree_node *leaf, uint32_t key, long offset);

#endif
