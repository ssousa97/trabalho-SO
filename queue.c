#include "queue.h"
#include "util.h"

queue* initQueue() {
    queue* q = malloc(sizeof(queue));
    q->processes = calloc(MAX_PROCESSES, sizeof(process*));
    q->size = 0;
    return q;
}

void reorder(queue* q, int index) {
    for (; index < q->size - 1; ++index) {
        q->processes[index] = q->processes[index+1];
    }
    q->processes[q->size-1] = NULL;
    q->size--;
}

void insert(queue* q, process* p) {
    q->processes[q->size] = p;
    q->size++;
}

process* next(queue* q) {
    process* p = q->processes[0];
    reorder(q, 0);
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

void removeAt(queue* q, int index) {
    if (q->size > index)
        reorder(q, index);
}