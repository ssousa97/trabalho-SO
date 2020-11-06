#include "queue.h"
#include "util.h"

queue* initQueue() {
    queue* q = malloc(sizeof(queue));
    q->processes = calloc(MAX_PROCESSES, sizeof(process*));
    q->size = 0;
    return q;
}

void insert(queue* q, process* p) {
    q->processes[q->size] = p;
    q->size++;
}

process* next(queue* q) {
    process* p = q->processes[0];
    for (int i = 0; i < q->size - 1; ++i) {
        q->processes[i] = q->processes[i+1];
    }
    q->processes[q->size] = NULL;
    q->size--;
    return p;
}

process* peek(queue* q) {
    if (q->size > 0)
        return q->processes[0];
    return NULL;
}

process* peekIndex(queue* q, int index) {
    if (q->size > index)
        return q->processes[index];
    return NULL;
}