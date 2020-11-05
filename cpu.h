#ifndef CPU_H
#define CPU_H

#include "process.h"
#include "queue.h"

typedef struct {

    int cycles;
    int quantum;
    int elapsedTime;
    process* executingProcess;
    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* IOPriorityQueue;

} cpu_t;

cpu_t* initCPU();
void addProcessToQueueAfterIO(cpu_t*, process*);
void addProcessToQueue(cpu_t*, process*);
void addReadyProcessesToCPU(cpu_t*, process**);

#endif