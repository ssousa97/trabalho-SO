#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

#include "process.h"

typedef struct {

    process** processes;
    int size;

} queue;

void insertIntoQueue(queue*,process*);
process* nextFromQueue(queue*);
queue* initQueue();

#endif