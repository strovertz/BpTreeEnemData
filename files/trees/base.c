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
    if (!node) {
        *found_node = NULL;
        return -1;
    }

    int i = 0;
    while (i < node->nkeys && key > node->keys[i]) {
        i++;
    }

    if (node->is_leaf) {
    *found_node = node;  // SEMPRE define o nó folha
    for (int j = 0; j < node->nkeys; j++) {
        if (node->keys[j] == key)
            return j;
    }
    return -1;
    } else {
        if (i <= node->nkeys && node->children[i]) {
            return bptree_search(node->children[i], key, found_node);
        } else {
            *found_node = NULL;
            return -1;
        }
    }
}


void insert_into_leaf(bptree_node **root, bptree_node *leaf, uint32_t key, long offset) {
    if (leaf->nkeys < M - 1) {
        int i = 0;
        while (i < leaf->nkeys && leaf->keys[i] < key) i++;
        int j;
        for (j = leaf->nkeys; j > i; j--) {
            leaf->keys[j] = leaf->keys[j - 1];
            leaf->offsets[j] = leaf->offsets[j - 1];
        }
        leaf->keys[i] = key;
        leaf->offsets[i] = offset;
        leaf->nkeys++;
    } else {
        insert_into_leaf_after_splitting(root, leaf, key, offset);
    }
}


void insert_into_leaf_after_splitting(bptree_node **root, bptree_node *leaf, uint32_t key, long offset) {
    uint32_t temp_keys[M];
    long temp_offsets[M];
    int i, insertion_index = 0;

    while (insertion_index < M - 1 && leaf->keys[insertion_index] < key)
        insertion_index++;

    for (i = 0; i < leaf->nkeys; i++) {
        temp_keys[i] = leaf->keys[i];
        temp_offsets[i] = leaf->offsets[i];
    }

    for (i = M - 1; i > insertion_index; i--) {
        temp_keys[i] = temp_keys[i - 1];
        temp_offsets[i] = temp_offsets[i - 1];
    }

    temp_keys[insertion_index] = key;
    temp_offsets[insertion_index] = offset;

    int split = (M + 1) / 2;

    bptree_node *new_leaf = create_node(true);
    if (!new_leaf) {
        fprintf(stderr, "Erro ao alocar nova folha.\n");
        return;
    }

    leaf->nkeys = 0;
    for (i = 0; i < split; i++) {
        leaf->keys[i] = temp_keys[i];
        leaf->offsets[i] = temp_offsets[i];
        leaf->nkeys++;
    }

    for (i = split; i < M; i++) {
        new_leaf->keys[new_leaf->nkeys] = temp_keys[i];
        new_leaf->offsets[new_leaf->nkeys] = temp_offsets[i];
        new_leaf->nkeys++;
    }

    new_leaf->next = leaf->next;
    leaf->next = new_leaf;

    insert_into_parent(root, leaf, new_leaf->keys[0], new_leaf);
}

void insert_into_parent(bptree_node **root, bptree_node *left, uint32_t key, bptree_node *right) {
    if (*root == left) {
        bptree_node *new_root = create_node(false);
        new_root->keys[0] = key;
        new_root->children[0] = left;
        new_root->children[1] = right;
        new_root->nkeys = 1;
        *root = new_root;
        return;
    }

    bptree_node *parent = NULL;
    bptree_node *current = *root;
    while (!current->is_leaf) {
        for (int i = 0; i <= current->nkeys; i++) {
            if (current->children[i] == left) {
                parent = current;
                break;
            }
        }
        if (parent) break;
        int i = 0;
        while (i < current->nkeys && key > current->keys[i]) i++;
        current = current->children[i];
    }

    if (!parent) {
        fprintf(stderr, "Erro: pai não encontrado.\n");
        return;
    }

    if (parent->nkeys < M - 1) {
        int i = 0;
        while (i < parent->nkeys && parent->keys[i] < key) i++;
        int j;
        for ( j = parent->nkeys; j > i; j--) {
            parent->keys[j] = parent->keys[j - 1];
            parent->children[j + 1] = parent->children[j];
        }
        parent->keys[i] = key;
        parent->children[i + 1] = right;
        parent->nkeys++;
    } else {
        insert_into_internal_after_splitting(root, parent, key, right);
    }

}

void insert_into_internal_after_splitting(bptree_node **root, bptree_node *old_node, uint32_t key, bptree_node *right) {
    int i, j;
    uint32_t temp_keys[M];
    bptree_node *temp_children[M + 1];

    int insertion_index = 0;
    while (insertion_index < old_node->nkeys && old_node->keys[insertion_index] < key)
        insertion_index++;

    for (i = 0, j = 0; i < old_node->nkeys + 1; i++) {
        if (j == insertion_index + 1) j++;
        temp_children[j++] = old_node->children[i];
    }

    for (i = 0, j = 0; i < old_node->nkeys; i++) {
        if (j == insertion_index) j++;
        temp_keys[j++] = old_node->keys[i];
    }

    temp_keys[insertion_index] = key;
    temp_children[insertion_index + 1] = right;

    int split = M / 2;
    bptree_node *new_node = create_node(false);
    if (!new_node) {
        fprintf(stderr, "Erro ao alocar novo nó interno.\n");
        return;
    }

    old_node->nkeys = 0;
    for (i = 0; i < split; i++) {
        old_node->keys[i] = temp_keys[i];
        old_node->children[i] = temp_children[i];
        old_node->nkeys++;
    }
    old_node->children[i] = temp_children[i];

    uint32_t k_prime = temp_keys[split];

    new_node->nkeys = 0;
    for (++i, j = 0; i < M; i++, j++) {
        new_node->keys[j] = temp_keys[i];
        new_node->children[j] = temp_children[i];
        new_node->nkeys++;
    }
    new_node->children[j] = temp_children[i];

    insert_into_parent(root, old_node, k_prime, new_node);
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
