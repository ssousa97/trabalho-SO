
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

    int duration;
    int startingTime;
    int elapsedTime;
    int pid;
    int ppid;
    int returnedFromIO;
    int preempted;
    PRIORITY priority;
    STATUS status;
    IO_TYPE ioType;

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
int generateRandomPriority();
int generateRandomIO();
process* initRandomProcess();
process** initRandomProcesses();
int allProcessFinished(process**);

void freeProcesses(process**);

#endif