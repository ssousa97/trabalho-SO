#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//definições
typedef enum {IO, HIGH, LOW} PRIORITY;
typedef enum {DISK, MAGNETIC_TAPE, PRINTER, NONE} IO_TYPE;
typedef enum {READY, BLOCKED, RUNNING} STATUS;

typedef struct {

    int duration;
    int pid;
    int ppid;
    PRIORITY priority;
    STATUS status;
    IO_TYPE ioType;

} process;

typedef struct  {

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
int generateRandomPID();
int generateRandomTime();


const char* getPriorityAsString(int priority){
    switch(priority){
        case HIGH: return "High";
        case LOW: return "Low";
        case IO: return "IO";
        default : break;
    }
}

const char* getStatusAsString(int status){
    switch(status){
        case READY: return "Ready";
        case RUNNING: return "Running";
        case BLOCKED: return "Blocked";
        default : break;
    }
}

const char* getIoTypeAsString(int io_type){
    switch(io_type){
        case DISK : return "Disk";
        case MAGNETIC_TAPE : return "Magnetic Tape";
        case PRINTER : return "Printer";
        case NONE : return "None";
        default : break;
    }

}

void printProcess(process* p){
    printf("PID : %d\nDuration : %d\nPPID : %d\nPRIORITY : %s\nSTATUS : %s\nIO TYPE : %s\n\n",
    p->pid,
    p->duration,
    p->ppid,
    getPriorityAsString(p->priority),
    getStatusAsString(p->status),
    getIoTypeAsString(p->ioType));
}

int generateRandomPID(){
    return rand() % 1000;
}

int generateRandomTime(){
    return (rand() % 16) + 1;
}

int main(){

    srand(time(0));

    queue* highPriorityQueue;
    queue* lowPriorityQueue;
    queue* IOPriorityQueue;

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