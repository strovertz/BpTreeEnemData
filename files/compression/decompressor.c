#include <stdio.h>
#include <stdlib.h>

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
        if (count == EOF) break;

        for (int i = 0; i < count; i++) {
            fputc(ch, out);
        }
    }

    fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.rle> <arquivo_saida.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    decompress_file(argv[1], argv[2]);
    printf("Descompressão concluída: %s → %s\n", argv[1], argv[2]);

    return EXIT_SUCCESS;
}
