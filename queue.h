#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

#include "process.h"

typedef struct {

    process** processes;
    int size;

} queue;

void insert(queue*,process*);
process* next(queue*);
queue* initQueue();
process* peek(queue*);
process* peekIndex(queue*, int);

#endif