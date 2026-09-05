#include <string.h>
#include "scheduler.h"

int main (int argc, char **argv) {
    Task t[1000] = {0};
    int n = 0;
    int tempo_total = 0;

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

    if (validarArquivo(f, t, &n, &tempo_total) != 0) {
        fclose(f);
        return 1;
    }
 
    if (simular(argv[1], t, n, tempo_total) != 0) {
        fclose(f);
        return 1;
    }
    fclose(f);

    return 0;
}

int validarArquivo(FILE *f, Task *t, int *n, int *tempo_total) {    
    if (*n >= 1000) {
        fprintf(stderr, "Erro: muitas tarefas (máx 1000)\n");
        return 1;
    }
    
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
            *tempo_total = total;
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
            strcpy(t[*n].nome, nome);
            t[*n].P = P;
            t[*n].D = D;
            t[*n].C = C;
            (*n)++;
        }
        linha++;
    }
    if (linha == 1) {
        fprintf(stderr, "Erro: arquivo vazio\n");
        return 1;
    }
    
    return 0;
}

int simular(char *modo, Task *t, int n, int total) {
    int is_rate = (strcmp(modo, "rate") == 0);
    int t_atual;
    int rodando = -1;
    int conta = 0;

    char nome[50];
    if (is_rate) {
        strcpy(nome, "rate_gnfr.out");
    } else {
        strcpy(nome, "edf_gnfr.out");
    }

    FILE *f = fopen(nome, "w");
    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", nome);
        perror("");
        return 1;
    }

    fprintf(f, "EXECUTION BY %s\n\n", is_rate ? "RATE" : "EDF");

    for (t_atual = 0; t_atual < total; t_atual++) {
        for (int i = 0; i < n; i++) {
            if (t[i].ativa && t_atual == t[i].deadline_abs && t[i].restante > 0) {
                t[i].ativa = 0;
                t[i].restante = 0;
                t[i].perdidas++;
            }
        }
        
        for (int i = 0; i < n; i++) {
            if (t_atual % t[i].P == 0) {
                t[i].ativa = 1;
                t[i].restante = t[i].C;
                t[i].deadline_abs = t_atual + t[i].D;
            }
        }

        int melhor = -1;
        for (int i = 0; i < n; i++) {
            if (!t[i].ativa) continue;
            if (melhor == -1) {
                melhor = i;
                continue;
            }
            
            if (is_rate) {
                if (t[i].P < t[melhor].P) melhor = i;
            } else {
                if (t[i].deadline_abs < t[melhor].deadline_abs) melhor = i;
            }
        }

        if (melhor != -1) {
            t[melhor].restante--;
            if (t[melhor].restante == 0) {
                t[melhor].ativa = 0;
                t[melhor].completas++;
            }
        }

        if (melhor != rodando) {
            if (conta > 0) {
                if (rodando == -1)
                    fprintf(f, "idle for %d units\n", conta);
                else if (t[rodando].ativa)
                    fprintf(f, "[%s] for %d units - H\n", t[rodando].nome, conta);
                else
                    fprintf(f, "[%s] for %d units - L\n", t[rodando].nome, conta);
            }
            rodando = melhor;  
            conta = 1;          
        } else {
            conta++;            
        }

        if (melhor != -1 && t[melhor].restante == 0) {
            fprintf(f, "[%s] for %d units - F\n", t[melhor].nome, conta);
            rodando = -1;
            conta = 0;
        }
    }

    if (conta > 0) {
        if (rodando == -1)
            fprintf(f, "idle for %d units\n", conta);
        else if (t[rodando].restante == 0)
            fprintf(f, "[%s] for %d units - F\n", t[rodando].nome, conta);   
        else
            fprintf(f, "[%s] for %d units - H\n", t[rodando].nome, conta);   
    }

    for (int i = 0; i < n; i++) {
        if (t[i].ativa && t[i].restante > 0) t[i].mortas++;
    }

    fprintf(f, "\nLOST DEADLINES\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "[%s] %d\n", t[i].nome, t[i].perdidas);
    }

    fprintf(f, "\nCOMPLETE EXECUTION\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "[%s] %d\n", t[i].nome, t[i].completas);
    }

    fprintf(f, "\nKILLED\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "[%s] %d\n", t[i].nome, t[i].mortas);
    }

    fclose(f);
    return 0;
}
