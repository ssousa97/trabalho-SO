#ifndef CPU_H
#define CPU_H

#include "process.h"
#include "queue.h"

#define QUANTUM 5

typedef struct {

    int cycles;
    int quantum;
    int elapsedTime;
    process* executingProcess;
    queue** startingTimeTable;
    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* IOPriorityQueue;

} cpu_t;

cpu_t* initCPU();
void addProcessToQueueAfterIO(cpu_t*, process*);
void addProcessToQueue(cpu_t*, process*);
void sendReadyProcessesToCPU(cpu_t*, process**);
void roundRobin(cpu_t*);

void freeCPU(cpu_t*);

#endif