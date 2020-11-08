#include "queue.h"
#include "util.h"

queue* initQueue() {
    // Inicializa fila
    // A fila pode ter até MAX_PROCESSES
    // então já pré alocamos a quantidade de processos necessários
    // Desse momento em frente, manteremos a fila apenas alterando o campo
    // size 
    queue* q = malloc(sizeof(queue));
    q->processes = calloc(MAX_PROCESSES, sizeof(process*));
    q->size = 0;
    return q;
}

void reorder(queue* q, int index) {
    // Reordena a fila a partir de um index
    // Fazemos a reordenação par a par
    // Com q = [1, 3, 2, 5], reorder(q, 0) ficaria:
    // q = [3, 2, 5]
    for (; index < q->size - 1; ++index) {
        q->processes[index] = q->processes[index+1];
    }
    q->processes[q->size-1] = NULL;
    q->size--;
}

void insert(queue* q, process* p) {
    // Insere processo na fila e aumenta seu tamanho
    q->processes[q->size] = p;
    q->size++;
}

process* next(queue* q) {
    // Consome próximo processo da fila
    process* p = q->processes[0];
    reorder(q, 0);
    return p;
}

process* peek(queue* q) {
    // Olha qual o próximo processo na fila
    if (q->size > 0)
        return q->processes[0];
    return NULL;
}

process* peekIndex(queue* q, int index) {
    // Olha qual o processo em um determinado index
    if (q->size > index)
        return q->processes[index];
    return NULL;
}

void removeAt(queue* q, int index) {
    // Remove um processo num determinado index
    if (q->size > index)
        reorder(q, index);
}