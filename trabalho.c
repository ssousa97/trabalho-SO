#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

#define MAX_PROCESSES 10
#define MAX_STARTING_TIME 16

#define PRIORITY_SIZE 3
#define IO_TYPE_SIZE 4

//definições
typedef enum {IO, HIGH, LOW} PRIORITY;
typedef enum {DISK, MAGNETIC_TAPE, PRINTER, NONE} IO_TYPE;
typedef enum {READY, BLOCKED, RUNNING, FINISHED} STATUS;

typedef struct {

    int duration;
    int pid;
    int ppid;
    PRIORITY priority;
    STATUS status;
    IO_TYPE ioType;

} process;

typedef struct {

    process* p;
    struct queue* next;
    struct queue* front;
    struct queue* back;

} queue;

//assinatura das funções
const char* getPriorityAsString(int);
const char* getStatusAsString(int);
const char* getIoTypeAsString(int);
void printProcess(process*);
pid_t generateRandomPID();
pid_t generateIncrementalPID();
int generateRandomTime();
int generateRandomPriority();
int generateRandomIO();
process* initRandomProcess(pid_t);
process** initRandomProcesses();

const char* getPriorityAsString(int priority) {
    switch(priority){
        case HIGH: return "High";
        case LOW: return "Low";
        case IO: return "IO";
        default : break;
    }
}

const char* getStatusAsString(int status) {
    switch(status){
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
    printf("PID : %d\nDuration : %d\nPPID : %d\nPRIORITY : %s\nSTATUS : %s\nIO TYPE : %s\n\n",
    p->pid,
    p->duration,
    p->ppid,
    getPriorityAsString(p->priority),
    getStatusAsString(p->status),
    getIoTypeAsString(p->ioType));
}

pid_t generateRandomPID() {
    return rand() % 1000;
}

pid_t generateIncrementalPID() {
    static pid_t lastPID = 0;
    return ++lastPID;
}

int generateRandomTime() {
    return (rand() % MAX_STARTING_TIME) + 1;
}

int generateRandomPriority() {
    return rand() % PRIORITY_SIZE;
}

int generateRandomIO() {
    return rand() % IO_TYPE_SIZE;
}

process* initRandomProcess(pid_t pid) {
    process* proc = malloc(sizeof(process));
    proc->duration = 0;
    proc->pid = pid;
    proc->ppid = 0;     // fix parent pid at 0, TODO change?
    proc->priority = generateRandomPriority();
    proc->status = READY;
    proc->ioType = generateRandomIO();
    return proc;
}

process** initRandomProcesses() {
    // Allocate processes in the heap
    process** initializedProcesses = calloc(MAX_PROCESSES, sizeof(process*));
    for (pid_t pid = generateIncrementalPID(); pid < MAX_PROCESSES; pid = generateIncrementalPID()) {
        initializedProcesses[pid] = initRandomProcess(pid);
        printProcess(initializedProcesses[pid]);
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

int main() {

    srand(time(0));

    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* IOPriorityQueue;

    process** processes = initRandomProcesses();

    // while(!allProcessFinished(processes)) {
    //     // do something with the processes
    // }

    // int amount;
    // printf("\nInsira a quantidade de processos que deseja criar : ");
    // scanf("%d",&amount);

    // int timeSlice;
    // printf("Insira o tamanho do quantum (time slice) : ");
    // scanf("%d",&timeSlice);

    printf("%d\n",generateRandomPID());
    printf("%d\n",generateRandomTime());

    return 0;
}