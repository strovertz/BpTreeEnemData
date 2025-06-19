# ENEM Microdados Processor

Este projeto processa e organiza dados de inscrições do **MICRODADOS_ENEM_2023.csv**. Ele estrutura as informações por estados, cidades e inscrições, permitindo buscas eficientes por matrícula, cidade + estado e filtragem por nota na redação.

# AVISOS

POR FAVOR, CONVERTER O .CSV PARA UTF-8 ANTES DE EXECUTAR.
``` bash
iconv -f ISO-8859-1 -t UTF-8 -o arquivo.csv MICRODADOS_ENEM_2023.csv
```
## Funcionalidades

- Carrega dados de inscrições do ENEM em uma árvore binária de busca organizada por estados e suas cidades.
- Insere e gerencia inscrições de alunos por cidade e estado.
- Busca inscrições por matrícula usando B+ tree.
- Lista inscrições por município e estado.
- Lista todas as inscrições com nota na redação maior ou igual a 900.
- Gerenciamento eficiente de memória e estruturas de dados para grandes volumes.

## Estrutura do Projeto

- **bst.h / bst.c**: Implementação da árvore binária de busca para estados e cidades.
- **base.h / base.c**: Lógica principal para inserir estados, cidades e inscrições.
- **main.c**: Ponto de entrada com interface para consulta de dados.
- **Makefile**: Instruções para compilação.
- **README.md**: Esta documentação.

### Estrutura dos Dados
Para organizar os dados, utilizamos:

Uma árvore binária de busca (BST) para armazenar os estados.

Cada estado contém um vetor de cidades, que armazenam as inscrições realizadas na respectiva cidade.

Para buscas rápidas por inscrição, implementamos uma árvore B+ indexando os offsets no arquivo.

### Estratégias de Busca
Busca por Inscrição
Utilizamos a árvore B+ para indexar as inscrições com seus respectivos offsets no arquivo CSV.

Ao buscar uma inscrição, localizamos o offset e fazemos um fseek no arquivo para leitura direta da linha desejada.

Busca por Cidade e Estado
Percorremos a árvore de estados para encontrar o nó do estado desejado.

Dentro do estado, buscamos a cidade pelo nome em um vetor.

Ao encontrar a cidade, listamos todas as inscrições associadas.

Listagem das Melhores Redações
As inscrições com nota >= 900 são armazenadas em uma lista dinâmica separada.

A listagem é feita diretamente exibindo essa lista, evitando percorrer toda a base.


## Como usar

1. Compile o projeto:

   ```bash
   gcc --ansi -Wall main.c ./files/trees/base.c ./files/lists/lista.c ./files/trees/bst.c -o enem_processor
Execute o programa:

bash
Copiar
Editar
./enem_processor
Siga o menu para:

Buscar inscrição por matrícula.

Listar inscrições por cidade e estado.

Mostrar todas as inscrições com nota na redação ≥ 900.

Informe os offsets quando solicitado para acessar rapidamente linhas específicas do arquivo.

Exemplo
diff
Copiar
Editar
Digite a sigla do estado (UF): RS
Digite o nome da cidade: Porto Alegre
Inscrições para Porto Alegre - RS:
- 123456
- 789012
