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

int getIODuration(int IOType) {
    // Retorna duração de tempo de cada IO
    // TODO: talvez colocar esses tempos como define
    switch(IOType) {
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
    // Printa descrição do processo
    printf("=== Process Description ===\n");
    printf("PID: %d\nStarting Time: %d\nElapsed time (CPU): %d\n"
            "Elapsed time (IO): %d\nDuration: %d\nIO Starting time: %d\n"
            "PPID: %d\nPRIORITY: %s\nSTATUS: %s\nIO TYPE: %s\n\n",
        p->pid,
        p->startingTime,
        p->elapsedTimeCPU,
        p->elapsedTimeIO,
        p->duration,
        p->IOStartingTime,
        p->ppid,
        getPriorityAsString(p->priority),
        getStatusAsString(p->status),
        getIoTypeAsString(p->IOType));
}

pid_t generateIncrementalPID() {
    // Retorna um PID incremental
    static pid_t lastPID = 0;
    return ++lastPID;
}

// Randomness generators
int generateRandomDuration() {
    // Retorna tempo de duração aleatorio do processo na CPU
    return (rand() % MAX_DURATION) + 1;
}

int generateRandomStartingTime() {
    // Retorna em qual ciclo o processo deverá ser iniciado
    return (rand() % MAX_STARTING_TIME) + 1;
}

int generateRandomIOStartingTime(int duration) {
    // Retorna em qual ciclo, após o processo entrar na CPU,
    // deverá começar a operação de IO.
    // Leva em conta que o tempo de inicio não poderá ser após
    // o tempo máximo de duração do processo
    return (rand() % (duration)) + 1;
}

int generateRandomPriority() {
    // Retorna prioridade aleatoria
    return rand() % PRIORITY_SIZE;
}

int generateRandomIO() {
    // Retorna um IO aleatorio
    return rand() % IO_TYPE_SIZE;
}

process* initRandomProcess() {
    // Inicializa processo aleatório individualmente
    process* proc = malloc(sizeof(process));
    proc->duration = generateRandomDuration();
    proc->startingTime = generateRandomStartingTime();
    proc->pid = generateIncrementalPID();
    proc->ppid = 1;     // fix parent at PID 1
    proc->priority = generateRandomPriority();
    proc->status = NOT_STARTED;
    proc->IOType = generateRandomIO();
    proc->IOStartingTime = proc->IOType != NONE
        ? generateRandomIOStartingTime(proc->duration)
        : -1;
    return proc;
}

process** initRandomProcesses() {
    // Inicializa processos aleatórios que serão utilizados na simulação
    process** initializedProcesses = calloc(MAX_PROCESSES, sizeof(process*));
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        initializedProcesses[i] = initRandomProcess();
        printProcess(initializedProcesses[i]);
    }
    return initializedProcesses;
}

int allProcessFinished(process** processes) {
    // Retorna true se todos os processos tiverem terminado
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i]->status != FINISHED)
            return FALSE;
    }
    return TRUE;
}

int hasQuantumExpired(process* proc, int quantum) {
    return proc->quantumCounter == quantum;
}

int hasProcessFinished(process* proc) {
    return proc->elapsedTimeCPU == proc->duration;
}

int hasIOFinished(process* IOProcess) {
    return IOProcess->elapsedTimeIO >= getIODuration(IOProcess->IOType);
}

int hasReachedIOTime(process* proc) {
    return proc->elapsedTimeCPU == proc->IOStartingTime;
}

void resetQuantum(process* proc) {
    proc->quantumCounter = 0;
}

void freeProcesses(process** processes) {
    for (int i = 0; i < MAX_PROCESSES; ++i)
        free(processes[i]);
    free(processes);
}