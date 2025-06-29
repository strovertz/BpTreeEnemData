#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>


#define MAX_LINE_LENGTH 8192
#define MAX_WORD_LEN 128
#define MAX_LINES 4000000
#define INITIAL_DICT_CAPACITY 10000

typedef struct {
    char **words;
    int size;
    int capacity;
} Dictionary;

void dict_init(Dictionary *dict) {
    dict->capacity = INITIAL_DICT_CAPACITY;
    dict->size = 0;
    dict->words = malloc(sizeof(char*) * dict->capacity);
    if (!dict->words) {
        fprintf(stderr, "Erro malloc dict\n");
        exit(EXIT_FAILURE);
    }
}

void dict_free(Dictionary *dict) {
    int i;
    for (i = 0; i < dict->size; i++) {
        free(dict->words[i]);
    }
    free(dict->words);
}

int dict_add(Dictionary *dict, const char *word) {
    int i;
    for (i = 0; i < dict->size; i++) {
        if (strcmp(dict->words[i], word) == 0)
            return i;
    }
    if (dict->size == dict->capacity) {
        dict->capacity *= 2;
        dict->words = realloc(dict->words, sizeof(char*) * dict->capacity);
        if (!dict->words) {
            fprintf(stderr, "Erro realloc dict\n");
            exit(EXIT_FAILURE);
        }
    }
    dict->words[dict->size] = strdup(word);
    if (!dict->words[dict->size]) {
        fprintf(stderr, "Erro strdup\n");
        exit(EXIT_FAILURE);
    }
    return dict->size++;
}

void get_field(const char *line, int index, char *out_field, int max_len) {
    int current = 0;
    int i = 0;
    const char *p;
    for (p = line; *p && *p != '\n' && *p != '\r'; p++) {
        if (current == index) {
            if (*p == ';') break;
            if (i < max_len - 1) out_field[i++] = *p;
        }
        if (*p == ';') current++;
        if (current > index) break;
    }
    out_field[i] = '\0';
}

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    else {
        perror("stat erro");
        return -1;
    }
}

void decompress_file(const char *input_path, const char *output_path) {
    FILE *in = fopen(input_path, "rb");
    FILE *out = fopen(output_path, "wb");
    if (!in || !out) {
        perror("Erro ao abrir arquivos");
        exit(EXIT_FAILURE);
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        int count = fgetc(in);
        for (int i = 0; i < count; i++) {
            fputc(ch, out);
        }
    }

    fclose(in);
    fclose(out);
}


int main() {
    FILE *fin = fopen("../../arquivo.csv", "r");
    if (!fin) {
        perror("Erro abrindo arquivo de entrada");
        return EXIT_FAILURE;
    }

    FILE *fout = fopen("arquivo_comprimido.csv", "w");
    if (!fout) {
        perror("Erro criando arquivo de saída");
        fclose(fin);
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    char field13[MAX_WORD_LEN];
    char field15[MAX_WORD_LEN];

    Dictionary dict13, dict15;
    dict_init(&dict13);
    dict_init(&dict15);

    int line_num = 0;

    while (fgets(line, sizeof(line), fin)) {
        if (line_num == 0) {
            fputs(line, fout);
            line_num++;
            continue;
        }

        if (line_num >= MAX_LINES) break;

        get_field(line, 13, field13, MAX_WORD_LEN);
        get_field(line, 15, field15, MAX_WORD_LEN);

        int idx13 = dict_add(&dict13, field13);
        int idx15 = dict_add(&dict15, field15);


        int total_fields = 0;
        char *p;
        for (p = line; *p; p++) {
            if (*p == ';') total_fields++;
        }
        total_fields++;

        int current_field = 0;
        char field[MAX_WORD_LEN];
        int i = 0;
        char *p;
        for (p = line; ; p++) {
            if (*p == ';' || *p == '\0' || *p == '\n' || *p == '\r') {
                field[i] = '\0';
                if (current_field == 13) {
                    fprintf(fout, "%d", idx13);
                } else if (current_field == 15) {
                    fprintf(fout, "%d", idx15);
                } else {
                    fputs(field, fout);
                }

                current_field++;
                if (current_field < total_fields) {
                    fputc(';', fout);
                } else {
                    fputc('\n', fout);
                    break;
                }
                i = 0;
            } else {
                if (i < MAX_WORD_LEN - 1) {
                    field[i++] = *p;
                }
            }
        }

        line_num++;
        if (line_num % 100000 == 0) {
            printf("Linhas processadas: %d\n", line_num);
        }
    }
    long original_size = get_file_size("../../arquivo.csv");
    long compressed_size = get_file_size("arquivo_comprimido.csv");
    if (original_size > 0 && compressed_size > 0) {
        double ratio = (double)original_size / compressed_size;
        double percent = (1.0 - ((double)compressed_size / original_size)) * 100.0;
        printf("Tamanho Original: %ld bytes\n", original_size);
        printf("Tamanho Comprimido: %ld bytes\n", compressed_size);
        printf("Taxa de Compressão: %.2f\n", ratio);
        printf("Percentual de Compressão: %.2f%%\n", percent);
    }
    dict_free(&dict13);
    dict_free(&dict15);

    fclose(fin);
    fclose(fout);

    printf("Arquivo processado. Total linhas: %d\n", line_num);

    return 0;
}
