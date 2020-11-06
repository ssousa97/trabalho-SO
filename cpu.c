#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "cpu.h"
#include "util.h"

cpu_t* initCPU() {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    cpu->cycles = 0;
    cpu->quantum = QUANTUM;
    cpu->elapsedTime = 0;
    cpu->executingProcess = NULL;
    cpu->highPriorityQueue = initQueue();
    cpu->lowPriorityQueue = initQueue();
    cpu->IOPriorityQueue = initQueue();
    return cpu;
}

void insertAfterReturnFromIO(cpu_t* cpu, process* p) {
    switch(p->ioType) {
        case DISK:
            printf("inserindo na fila de" ANSI_COLOR_RED" baixa " ANSI_COLOR_RESET "prioridade, IO: Disk\n");
            insert(cpu->lowPriorityQueue, p);
            break;
        case MAGNETIC_TAPE:
        case PRINTER:
            printf("inserindo na fila de" ANSI_COLOR_GREEN " alta " ANSI_COLOR_RESET "prioridade, IO: %s\n",
                getIoTypeAsString(p->ioType));

            insert(cpu->highPriorityQueue, p);
        case NONE:
            break;
    }
}

void addProcessToQueue(cpu_t* cpu, process* p) {
    if(p->returnedFromIO) {
        printf("Processo %d retornou de IO, ", p->pid);
        insertAfterReturnFromIO(cpu, p);
        p->returnedFromIO = FALSE;
    }
    else if(p->preempted) {
        insert(cpu->lowPriorityQueue, p);
        p->preempted = FALSE;
    }
}

void createStartingTimeTableCPU(cpu_t *cpu, process** processes) {
    cpu->startingTimeTable = calloc(MAX_STARTING_TIME, sizeof(queue*));
    for (int i = 0; i < MAX_STARTING_TIME; ++i)
        cpu->startingTimeTable[i] = calloc(1, sizeof(queue));

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        process* proc = processes[i];
        insert(cpu->startingTimeTable[proc->startingTime], proc);
    }
}

void sendNewProcessToCPU(cpu_t* cpu) {
    queue* queueNewProcesses = cpu->startingTimeTable[cpu->cycles];
    while (queueNewProcesses->size > 0) {
        process* proc = next(queueNewProcesses);
        proc->status = READY;
        insert(cpu->highPriorityQueue, proc);
    }
}

void sendToLowPriorityQueue(cpu_t* cpu, process* proc) {
    proc->elapsedTime = 0;
    proc->status = READY;
    insert(cpu->lowPriorityQueue, proc);
}

void manageProcessRunning(cpu_t* cpu) {
    process* currentProcess = cpu->executingProcess;
    currentProcess->elapsedTime++;
    if (hasProcessFinished(currentProcess)) {
        currentProcess->status = FINISHED;
        printf("Processo %d terminou, alocando CPU para outro processo\n",
            currentProcess->pid);

        dispatchProcessToCPU(cpu);
    } else if (hasQuantumExpired(currentProcess, cpu->quantum)) {
        sendToLowPriorityQueue(cpu, currentProcess);
        dispatchProcessToCPU(cpu);
    }
    // Add handler for IO request
}

process* findNextProcess(cpu_t* cpu) {
    process* proc = NULL;
    if (cpu->highPriorityQueue->size > 0)
        proc = next(cpu->highPriorityQueue);
    else if (cpu->lowPriorityQueue->size > 0)
        proc = next(cpu->lowPriorityQueue);
    return proc;
}

void dispatchProcessToCPU(cpu_t* cpu) {
    process* nextProcess = findNextProcess(cpu);
    if (nextProcess) {
        printf("Enviando processo %d para a CPU\n", nextProcess->pid);
        cpu->executingProcess = nextProcess;
        cpu->executingProcess->status = RUNNING;
    } else {
        printf("Nenhum processo foi alocado na CPU no ciclo %d\n", cpu->cycles);
    }
}

// void handleIOProcesses(cpu_t* cpu) {
//     // Update IO elapsed time for each process in the IO Queue
//     // Assumes that IOs are not dependent of each other
//     for (int i = 0; i < cpu->IOPriorityQueue->size; ++i) {
//         process* IOProcess = peekIndex(cpu->IOPriorityQueue, i);
//         IOProcess->elapsedTime++;

//         // IO finished
//         if (IOProcess->elapsedTime >= getIODuration(IOProcess->ioType)) {
//             removeAt(cpu->IOPriorityQueue, i);
//         }
//     }
// }

void handleIOProcesses(cpu_t* cpu) {
    if (cpu->IOPriorityQueue->size > 0) {
        process* IOProcess = peek(cpu->IOPriorityQueue);
        IOProcess->elapsedTime++;
        if (hasIOFinished(IOProcess)) {
            next(cpu->IOPriorityQueue);
            IOProcess->elapsedTime = 0;
            IOProcess->status = READY;
            IOProcess->returnedFromIO = TRUE;
        }
    }
}

void roundRobin(cpu_t* cpu) {
    if(cpu->executingProcess) {
        manageProcessRunning(cpu);
    } else {
        dispatchProcessToCPU(cpu);
    }

    handleIOProcesses(cpu);
}

void freeCPU(cpu_t* cpu) {
    free(cpu->executingProcess);
    free(cpu->highPriorityQueue);
    free(cpu->lowPriorityQueue);
    free(cpu->IOPriorityQueue);
    free(cpu);
}