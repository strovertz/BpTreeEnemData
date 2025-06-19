#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

bptree_node* create_node(bool is_leaf) {
    bptree_node* node = malloc(sizeof(bptree_node));
    if (!node) return NULL;

    node->nkeys = 0;
    node->is_leaf = is_leaf;
    node->next = NULL;
    int i;
    for (i = 0; i < M; i++)
        node->children[i] = NULL;

    /* offsets só fazem sentido em folha, mas inicializa tudo */
    for (i = 0; i < M - 1; i++)
        node->offsets[i] = -1;

    return node;
}

#include <stdio.h>

/* Busca recursiva, retorna o nó folha onde a chave deveria estar*/
/* e a posição dentro do nó (-1 se não encontrou)*/
int bptree_search(bptree_node *node, uint32_t key, bptree_node **found_node) {
    if (!node) return -1;

    int i = 0;
    while (i < node->nkeys && key > node->keys[i]) {
        i++;
    }

    if (node->is_leaf) {
        /* procurar chave no nó folha */
        int j;
        for (j = 0; j < node->nkeys; j++) {
            if (node->keys[j] == key) {
                *found_node = node;
                return j;
            }
        }
        *found_node = NULL;
        return -1;
    } else {
        /* descer para filho adequado*/
        return bptree_search(node->children[i], key, found_node);
    }
}

void insert_into_leaf(bptree_node *leaf, uint32_t key, long offset) {
    int i = 0;
    /* acha posição pra inserir (ordem crescente) */
    while (i < leaf->nkeys && leaf->keys[i] < key) {
        i++;
    }
    /* shifta pra direita */
    int j;
    for (j = leaf->nkeys; j > i; j--) {
        leaf->keys[j] = leaf->keys[j - 1];
        leaf->offsets[j] = leaf->offsets[j - 1];
    }
    leaf->keys[i] = key;
    leaf->offsets[i] = offset;
    leaf->nkeys++;
}


void liberarBPTree(bptree_node* node) {
    if (node == NULL) return;

    /*libera os filho recursivamente se não for uma folha.*/
    if (!node->is_leaf) {
        int i;
        for (i = 0; i <= node->nkeys; i++) {
            liberarBPTree(node->children[i]);
        }
    }

    free(node);
}
