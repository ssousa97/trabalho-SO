#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "cpu.h"
#include "util.h"

cpu_t* initCPU() {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    cpu->cycles = 0;
    cpu->quantum = QUANTUM;
    cpu->executingProcess = NULL;
    cpu->highPriorityQueue = initQueue();
    cpu->lowPriorityQueue = initQueue();
    cpu->PrinterQueue = initQueue();
    cpu->MagneticTapeQueue = initQueue();
    cpu->DiskQueue = initQueue();
    return cpu;
}

void insertAfterReturnFromIO(cpu_t* cpu, process* p) {
    switch(p->ioType) {
        case DISK:
            printf("Inserindo processo %d na fila de" ANSI_COLOR_RED " baixa " ANSI_COLOR_RESET "prioridade, "
                   "retornando de IO: Disk\n", p->pid);
            insert(cpu->lowPriorityQueue, p);
            break;
        case MAGNETIC_TAPE:
        case PRINTER:
            printf("Inserindo processo %d na fila de" ANSI_COLOR_GREEN " alta " ANSI_COLOR_RESET "prioridade, "
                   "retornando de IO: %s\n", p->pid, getIoTypeAsStrig(p->ioType));

            insert(cpu->highPriorityQueue, p);
        case NONE:
            break;
    }
}

void createStartingTimeTableCPU(cpu_t *cpu, process** processes) {
    cpu->startingTimeTable = calloc(MAX_STARTING_TIME, sizeof(queue*));
    for (int i = 0; i < MAX_STARTING_TIME; ++i)
        cpu->startingTimeTable[i] = initQueue();

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        process* proc = processes[i];
        insert(cpu->startingTimeTable[proc->startingTime], proc);
    }
}

void sendNewProcessToCPU(cpu_t* cpu) {
    if (cpu->cycles < MAX_STARTING_TIME) {
        queue* queueNewProcesses = cpu->startingTimeTable[cpu->cycles];
        while (queueNewProcesses->size > 0) {
            process* proc = next(queueNewProcesses);
            proc->status = READY;
            insert(cpu->highPriorityQueue, proc);
        }
    }
}

void sendToLowPriorityQueue(cpu_t* cpu, process* proc) {
    // proc->elapsedTime = 0;
    printf("Enviando processo %d para a fila de baixa prioridade\n", proc->pid);
    proc->status = READY;
    insert(cpu->lowPriorityQueue, proc);
}

void sendProcessToIOQueue(cpu_t* cpu, process* proc) {
    printf("Enviando processo %d para a fila de %s\n", proc->pid,
        getIoTypeAsString(proc->ioType));
    switch(proc->ioType) {
        case PRINTER:
            insert(cpu->PrinterQueue, proc);
            break;
        case MAGNETIC_TAPE:
            insert(cpu->MagneticTapeQueue, proc);
            break;
        case DISK:
            insert(cpu->DiskQueue, proc);
            break;
    }
}

void manageProcessRunning(cpu_t* cpu) {
    process* currentProcess = cpu->executingProcess;
    currentProcess->elapsedTimeCPU++;

    if (hasQuantumExpired(currentProcess, cpu->quantum)) {
        sendToLowPriorityQueue(cpu, currentProcess);
        dispatchNextProcessToCPU(cpu);
    } else if (hasReachedIOTime(currentProcess)) {
        setProcessIOStatus(currentProcess);
        sendProcessToIOQueue(cpu, currentProcess);
    } else if (hasProcessFinished(currentProcess)) {
        currentProcess->status = FINISHED;
        printf("Processo %d terminou, alocando CPU para outro processo\n",
            currentProcess->pid);

        dispatchNextProcessToCPU(cpu);
    }
}

process* findNextProcess(cpu_t* cpu) {
    process* proc = NULL;
    if (cpu->highPriorityQueue->size > 0)
        proc = next(cpu->highPriorityQueue);
    else if (cpu->lowPriorityQueue->size > 0)
        proc = next(cpu->lowPriorityQueue);
    return proc;
}

void dispatchNextProcessToCPU(cpu_t* cpu) {
    process* nextProcess = findNextProcess(cpu);
    if (nextProcess) {
        printf("Enviando processo %d para a CPU\n", nextProcess->pid);
        nextProcess->elapsedTimeCPU++;
        cpu->executingProcess = nextProcess;
        cpu->executingProcess->status = RUNNING;
    } else {
        printf("Nenhum processo foi alocado na CPU no ciclo %d\n", cpu->cycles);
    }
}

void handleIOProcesses(cpu_t* cpu) {
    handleIOQueue(cpu, cpu->PrinterQueue);
    handleIOQueue(cpu, cpu->MagneticTapeQueue);
    handleIOQueue(cpu, cpu->DiskQueue);
}

void handleIOQueue(cpu_t* cpu, queue* IOQueue) {
    if (IOQueue->size > 0) {
        process* IOProcess = peek(IOQueue);
        IOProcess->elapsedTimeIO++;
        if (hasIOFinished(IOProcess)) {
            next(IOQueue);
            IOProcess->status = READY;
            insertAfterReturnFromIO(cpu, IOProcess);
        }
    }
}

void roundRobin(cpu_t* cpu) {
    if(cpu->executingProcess) {
        manageProcessRunning(cpu);
    } else {
        dispatchNextProcessToCPU(cpu);
    }

    handleIOProcesses(cpu);
}

void freeCPU(cpu_t* cpu) {
    free(cpu->executingProcess);
    free(cpu->highPriorityQueue);
    free(cpu->lowPriorityQueue);
    free(cpu->PrinterQueue);
    free(cpu->MagneticTapeQueue);
    free(cpu->DiskQueue);
    free(cpu);
}