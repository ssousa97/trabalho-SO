#include "queue.h"
#include "util.h"

queue* initQueue() {
    queue* q = malloc(sizeof(queue));
    q->processes = calloc(MAX_PROCESSES, sizeof(process*));
    q->size = 0;
    return q;
}

void insertIntoQueue(queue* q, process* p) {
    q->processes[q->size] = p;
    q->size++;
}

process* nextFromQueue(queue* q) {
    process* p = q->processes[0];
    for (int i = 0; i < q->size - 1; ++i) {
        q->processes[i] = q->processes[i+1];
    }
    return p;
}
