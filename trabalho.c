#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

// ANSI colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TRUE 1
#define FALSE 0

#define MAX_PROCESSES 10
#define MAX_STARTING_TIME 16
#define MAX_DURATION 10

#define PRIORITY_SIZE 3
#define IO_TYPE_SIZE 4

//definições
typedef enum {IO, HIGH, LOW} PRIORITY;
typedef enum {DISK, MAGNETIC_TAPE, PRINTER, NONE} IO_TYPE;
typedef enum {NOT_STARTED, READY, BLOCKED, RUNNING, FINISHED} STATUS;

typedef struct {

    int duration;
    int startingTime;
    int pid;
    int ppid;
    int returnedFromIO;
    int preempted;
    PRIORITY priority;
    STATUS status;
    IO_TYPE ioType;

} process;

typedef struct {

    process** processes;
    int size;

} queue;

typedef struct {

    int cycles;
    int quantum;
    int elapsedTime;
    process* executingProcess;
    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* IOPriorityQueue;

} cpu_t;

//assinatura das funções
const char* getPriorityAsString(int);
const char* getStatusAsString(int);
const char* getIoTypeAsString(int);
void printProcess(process*);
pid_t generateRandomPID();
pid_t generateIncrementalPID();
int generateRandomDuration();
int generateRandomStartingTime();
int generateRandomPriority();
int generateRandomIO();
process* initRandomProcess();
process** initRandomProcesses();
int allProcessFinished(process**);
void insertIntoQueue(queue*,process*);
process* nextFromQueue(queue*);
void addProcessToQueueAfterIO(cpu_t*, process*);
void addProcessToQueue(cpu_t*, process*);
void addReadyProcessesToCPU(cpu_t*, process**);

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
    proc->ppid = 0;     // fix parent pid at 0, TODO: change?
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

queue* initQueue() {
    queue* q = malloc(sizeof(queue));
    q->processes = calloc(MAX_PROCESSES, sizeof(process*));
    q->size = 0;
    return q;
}

cpu_t* initCPU() {
    cpu_t* cpu = malloc(sizeof(cpu_t));
    cpu->highPriorityQueue = initQueue();
    cpu->lowPriorityQueue = initQueue();
    cpu->IOPriorityQueue = initQueue();
    cpu->cycles = 0;
    return cpu;
}

int allProcessFinished(process** processes) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i]->status != FINISHED)
            return FALSE;
    }
    return TRUE;
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

int main() {

    srand(time(0));

    cpu_t* cpu = initCPU();
    process** processes = initRandomProcesses();

    while(!allProcessFinished(processes)) {
        printf(ANSI_COLOR_BLUE "Ciclo %d\n" ANSI_COLOR_RESET, cpu->cycles++);

        addReadyProcessesToCPU(cpu, processes);

        // handle ready processes
    }

    // int amount;
    // printf("\nInsira a quantidade de processos que deseja criar : ");
    // scanf("%d",&amount);

    // int timeSlice;
    // printf("Insira o tamanho do quantum (time slice) : ");
    // scanf("%d",&timeSlice);

    // printf("%d\n",generateRandomPID());
    // printf("%d\n",generateRandomTime());

    return 0;
}