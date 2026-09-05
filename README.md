# Scheduler — Escalonamento de tarefas críticas de voo

Simulador em C que compara dois escalonadores de tempo real — **rate-monotonic** (`rate`) e **earliest-deadline-first** (`edf`) — sobre tarefas periódicas em que o deadline é menor que o período (C ≤ D ≤ P). O resultado é gravado em `algoritmo_gnfr.out` (`rate_gnfr.out` ou `edf_gnfr.out`), sem imprimir nada no stdout.

## Compilar

```bash
make clean
make
```

Gera o executável `scheduler` com `gcc -Wall -Wextra`.

## Uso

```bash
./scheduler rate voo.txt   # gera rate_gnfr.out
./scheduler edf  voo.txt   # gera edf_gnfr.out
```

## Formato de entrada

A primeira linha é o tempo total de simulação; cada linha seguinte é uma tarefa `NOME PERÍODO DEADLINE BURST` (todos inteiros positivos, com C ≤ D ≤ P):

```
100
ATT 20 12 8
NAV 50 30 15
```

Todas as tarefas chegam pela primeira vez no instante 0.

## Formato de saída

```
EXECUTION BY RATE

[ATT] for 8 units - F
[NAV] for 12 units - H
...
idle for 10 units

LOST DEADLINES
[ATT] 0
[NAV] 1

COMPLETE EXECUTION
[ATT] 5
[NAV] 1

KILLED
[ATT] 0
[NAV] 0
```

- `F` = a tarefa terminou a rajada.
- `H` = foi preemptada por uma tarefa de maior prioridade.
- `L` = perdeu o deadline no instante exato (rajada restante descartada).
- `idle` = nenhuma tarefa pronta.

## Como funciona

Ambos os algoritmos são preemptivos; a cada unidade de tempo roda a tarefa pronta de maior prioridade:

- **rate**: prioridade fixa — menor período vence.
- **edf**: prioridade dinâmica — menor deadline absoluto (chegada + D) vence.
- **Desempate**: nos dois algoritmos, a tarefa que aparece primeiro no arquivo.

Se uma instância não termina até o deadline absoluto, ela é perdida no instante exato do deadline: a rajada restante é descartada e a tarefa só volta a concorrer na próxima instância.

## Tratamento de erros

Entrada inválida escreve uma mensagem em `stderr`, encerra com código de saída ≠ 0 e **não cria** arquivo de saída. São tratados:

- número incorreto de argumentos;
- primeiro argumento diferente de `rate`/`edf`;
- arquivo inexistente ou ilegível;
- arquivo malformado (campo faltando, valor não numérico, valor não positivo);
- tarefa com D > P ou C > D;
- arquivo vazio.

## Estrutura

| Arquivo | Responsabilidade |
|---|---|
| `scheduler.c` | validação, simulação e escrita da saída |
| `scheduler.h` | struct `Task` e protótipos |
| `Makefile` | compilação |
