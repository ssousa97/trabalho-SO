#ifndef CPU_H
#define CPU_H

#include "process.h"
#include "queue.h"

typedef struct {

    int cycles;
    int quantum;
    process* executingProcess;
    queue** startingTimeTable;
    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* PrinterQueue;
    queue* MagneticTapeQueue;
    queue* DiskQueue;

} cpu_t;

cpu_t* initCPU();
void insertAfterReturnFromIO(cpu_t*, process*);
void addProcessToQueue(cpu_t*, process*);
void createStartingTimeTableCPU(cpu_t*, process**);
void sendNewProcessToCPU(cpu_t*);
void sendToLowPriorityQueue(cpu_t*, process*);
void manageProcessRunning(cpu_t*);
process* findNextProcess(cpu_t*);
void dispatchNextProcessToCPU(cpu_t*);
void handleIOProcesses(cpu_t*);
void handleIOQueue(cpu_t*, queue*);
void roundRobin(cpu_t*);

void freeCPU(cpu_t*);

#endif