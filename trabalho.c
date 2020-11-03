#include <stdio.h>
#include <stdlib.h>

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
    
} queue;

//assinatura das funções
const char* getPriorityAsString(int);
const char* getStatusAsString(int);
const char* getIoTypeAsString(int);
void printProcess(process*);

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

int main(){

    int amount;
    
    printf("\nInsira a quantidade de processos que deseja criar : ");
    scanf("%d",&amount);

    for(int i = 0; i < amount; i++){

    }


    return 0;
}