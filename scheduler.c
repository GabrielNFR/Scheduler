#include <string.h>
#include "scheduler.h"

int main (int argc, char **argv) {
    
    if (argc != 3) {
        fprintf(stderr, "Uso: ./scheduler <modo> <arquivo.txt>\n");
        return 1;
    }

    if (strcmp(argv[1],  "rate") != 0 && strcmp(argv[1], "edf") != 0) {
        fprintf(stderr, "Erro: modo não existente. Modos suportados: 'rate' e 'edf'\n");
        return 1;
    }

    FILE *f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", argv[2]);
        perror("");
        return 1;
    }

    if (validarArquivo(f) != 0) {
        fclose(f);
        return 1;
    }
    rewind(f);

    

    fclose(f);

    return 0;
}

int validarArquivo(FILE *f) {
    char line[256];
    int linha = 1;
    while (fgets(line, sizeof(line), f) != NULL) {
        if (linha == 1) {
            int total;
            char resto;
            int campos = sscanf(line, "%d %c", &total, &resto);

            if (campos != 1 || total <= 0) {
                fprintf(stderr, "Erro: primeira linha inválida\n");
                return 1;
            }
        }
        else {
            char nome[100];
            char resto;
            int P, D, C;
            int campos = sscanf(line, "%s %d %d %d %c", nome, &P, &D, &C, &resto);

            if (campos != 4 || P <= 0 || D <= 0 || C <= 0 || C > D || D > P) {
                fprintf(stderr, "Erro: linha %d inválida\n", linha);
                return 1;
            }
        }
        linha++;
    }
    if (linha == 1) {
        fprintf(stderr, "Erro: arquivo vazio\n");
        return 1;
    }

    return 0;
}