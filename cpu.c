#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "cpu.h"
#include "util.h"

cpu_t* initCPU() {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    cpu->highPriorityQueue = initQueue();
    cpu->lowPriorityQueue = initQueue();
    cpu->IOPriorityQueue = initQueue();
    cpu->cycles = 0;
    return cpu;
}

void insertIntoQueueAfterIO(cpu_t* cpu, process* p) {
    switch(p->ioType) {
        case DISK:
            insertIntoQueue(cpu->lowPriorityQueue, p);
            break;
        case MAGNETIC_TAPE:
        case PRINTER:
            insertIntoQueue(cpu->highPriorityQueue, p);
        case NONE:
            break;
    }
}

void addProcessToQueue(cpu_t* cpu, process* p) {
    if(p->returnedFromIO) {
        insertIntoQueueAfterIO(cpu, p);
        p->returnedFromIO = FALSE;
    }
    else if(p->preempted) {
        insertIntoQueue(cpu->lowPriorityQueue, p);
        p->preempted = FALSE;
    }
}

void addReadyProcessesToCPU(cpu_t* cpu, process** processes) {
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        // check if process has reached its starting time
        if (processes[i]->startingTime == cpu->cycles) {
            printf("Adicionando processo %d a fila de processos prontos com alta prioridade\n",
                processes[i]->pid);
            processes[i]->status = READY;
            insertIntoQueue(cpu->highPriorityQueue, processes[i]);
        } else if(processes[i]->status == READY)
            addProcessToQueue(cpu, processes[i]);
    }
}
