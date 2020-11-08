
#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define PRIORITY_SIZE 3
#define IO_TYPE_SIZE 4

typedef enum {IO, HIGH, LOW} PRIORITY;
typedef enum {DISK, MAGNETIC_TAPE, PRINTER, NONE} IO_TYPE;
typedef enum {NOT_STARTED, READY, BLOCKED, RUNNING, FINISHED} STATUS;

typedef struct {

    // fields fixed at init
    int duration;
    int startingTime;
    int IOStartingTime;
    int pid;
    int ppid;
    PRIORITY priority;
    STATUS status;
    IO_TYPE IOType;

    // variable fields
    int elapsedTimeCPU;
    int elapsedTimeIO;
    int quantumCounter;
    // int returnedFromIO;
    // int preempted;

} process;

const char* getPriorityAsString(int);
const char* getStatusAsString(int);
const char* getIoTypeAsString(int);
int getIODuration(int);
void printProcess(process*);
pid_t generateRandomPID();
pid_t generateIncrementalPID();
int generateRandomDuration();
int generateRandomStartingTime();
int generateRandomIOStartingTime();
int generateRandomPriority();
int generateRandomIO();
process* initRandomProcess();
process** initRandomProcesses();
int allProcessFinished(process**);
int hasQuantumExpired(process*, int);
int hasProcessFinished(process*);
int hasIOFinished(process*);
int hasReachedIOTime(process*);
void resetQuantum(process*);
void freeProcesses(process**);

#endif