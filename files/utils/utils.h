#ifndef UTILS_H
#define UTILS_H

void buscar_por_inscricao(FILE *filePtr, bptree_node *root);
void buscar_por_cidade_estado(Estados *estados);
void listar_melhores_redacoes(Redacoes *redacoes);
void menu_busca(FILE *filePtr, Estados *estados, bptree_node *root, Redacoes *redacoes);
bool is_numeric(const char *str);
int parse_nota_redacao(const char *nota_str);
char* getfield(const char* line, int column);
void imprimir_linha_do_offset(FILE *arquivo, long offset);
Estados* insere_cidades(Estados *Estados, const char* line, inscricao_t inscricao);


#endif /* UTILS_H */
