#include "process.h"
#include "util.h"

const char* getPriorityAsString(int priority) {
    switch(priority){
        case HIGH: return "High";
        case LOW: return "Low";
        case IO: return "IO";
        default : break;
    }
}

const char* getStatusAsString(int status) {
    switch(status) {
        case NOT_STARTED: return "Not started";
        case READY: return "Ready";
        case RUNNING: return "Running";
        case BLOCKED: return "Blocked";
        case FINISHED: return "Finished";
        default : break;
    }
}

const char* getIoTypeAsString(int io_type) {
    switch(io_type){
        case DISK : return "Disk";
        case MAGNETIC_TAPE : return "Magnetic Tape";
        case PRINTER : return "Printer";
        case NONE : return "None";
        default : break;
    }

}

int getIODuration(int ioType) {
    switch(ioType) {
        case DISK:
            return 5;
        case MAGNETIC_TAPE:
            return 15;
        case PRINTER:
            return 10;
        default:
            return 0;
    }
}

void printProcess(process* p) {
    printf("=== Process Description ===\n");
    printf("PID: %d\nDuration: %d\nPPID: %d\nPRIORITY: %s\nSTATUS: %s\nIO TYPE: %s\n\n",
        p->pid,
        p->duration,
        p->ppid,
        getPriorityAsString(p->priority),
        getStatusAsString(p->status),
        getIoTypeAsString(p->ioType));
}

// generate PID incrementally
pid_t generateIncrementalPID() {
    static pid_t lastPID = 0;
    return ++lastPID;
}

// Randomness generators
pid_t generateRandomPID() {
    return rand() % 1000;
}

int generateRandomDuration() {
    return (rand() % MAX_DURATION) + 1;
}

int generateRandomStartingTime() {
    return (rand() % MAX_STARTING_TIME) + 1;
}

int generateRandomPriority() {
    return rand() % PRIORITY_SIZE;
}

int generateRandomIO() {
    return rand() % IO_TYPE_SIZE;
}

// Process initializers
process* initRandomProcess() {
    process* proc = malloc(sizeof(process));
    proc->duration = generateRandomDuration();
    proc->startingTime = generateRandomStartingTime();
    proc->pid = generateIncrementalPID();
    proc->ppid = 1;     // fix parent at PID 1
    proc->priority = generateRandomPriority();
    proc->status = NOT_STARTED;
    proc->ioType = generateRandomIO();
    return proc;
}

process** initRandomProcesses() {
    // Allocate processes in the heap
    process** initializedProcesses = calloc(MAX_PROCESSES, sizeof(process*));
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        initializedProcesses[i] = initRandomProcess();
        printProcess(initializedProcesses[i]);
    }
    return initializedProcesses;
}

int allProcessFinished(process** processes) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i]->status != FINISHED)
            return FALSE;
    }
    return TRUE;
}

void freeProcesses(process** processes) {
    for (int i = 0; i < MAX_PROCESSES; ++i)
        free(processes[i]);
    free(processes);
}