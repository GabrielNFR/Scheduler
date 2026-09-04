#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>

typedef struct {
    char nome[100];
    int P, D, C;
    int completas, perdidas, mortas;
    int restante;
    int deadline_abs;
    int ativa;
} Task;

int validarArquivo(FILE *f, Task *t, int *n, int *total);
int simular(char *modo, Task *t, int n, int total);

#endif