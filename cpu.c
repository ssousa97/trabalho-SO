#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "cpu.h"
#include "util.h"

cpu_t* initCPU() {
    // Inicializa CPU e as filas necessárias para o funcionamento dela
    cpu_t* cpu = malloc(sizeof(cpu_t));
    cpu->cycles = 0;
    cpu->quantum = QUANTUM;
    cpu->executingProcess = NULL;
    cpu->highPriorityQueue = initQueue();
    cpu->lowPriorityQueue = initQueue();
    cpu->PrinterQueue = initQueue();
    cpu->MagneticTapeQueue = initQueue();
    cpu->DiskQueue = initQueue();
    return cpu;
}

void insertAfterReturnFromIO(cpu_t* cpu, process* p) {
    // Insere um processo `p` que retornou de IO na fila que deve ser inserido
    // de acordo com a especificação do trabalho:
    //  Disco: baixa prioridade
    //  Fita magnética: alta prioridade
    //  Impressora: alta prioridade
    switch(p->IOType) {
        case DISK:
            printf("Inserindo processo %d na fila de" ANSI_COLOR_RED " baixa " ANSI_COLOR_RESET "prioridade, "
                   "retornando de IO: Disk\n", p->pid);
            insert(cpu->lowPriorityQueue, p);
            break;
        case MAGNETIC_TAPE:
        case PRINTER:
            printf("Inserindo processo %d na fila de" ANSI_COLOR_GREEN " alta " ANSI_COLOR_RESET "prioridade, "
                   "retornando de IO: %s\n", p->pid, getIoTypeAsString(p->IOType));

            insert(cpu->highPriorityQueue, p);
        case NONE:
            break;
    }
}

void createStartingTimeTableCPU(cpu_t *cpu, process** processes) {
    // Para evitar que tenhamos que checar se ainda existem processos
    // no estado NOT_STARTED a cada ciclo, criamos uma "hash table"
    // que mapeia o tempo de início de cada processo numa fila de processos
    // Com os processos:
    //      1: startingTime = 3
    //      2: startingTime = 1
    //      3: startingTime = 3
    // Denotando {1,3} como uma fila que tem 1 na primeira posição
    // e 3 na segunda posição
    // E MAX_STARTING_TIME de 5
    // Teríamos uma "tabela" do tipo:
    // filas: [{}, {2}, {}, {1,3}, {}, {}]
    // index:  0 ,  1 , 2 ,   3  , 4 , 5
    cpu->startingTimeTable = calloc(MAX_STARTING_TIME+1, sizeof(queue*));
    for (int i = 1; i < MAX_STARTING_TIME + 1; ++i)
        cpu->startingTimeTable[i] = initQueue();

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        process* proc = processes[i];
        insert(cpu->startingTimeTable[proc->startingTime], proc);
    }
}

void sendNewProcessToCPU(cpu_t* cpu) {
    // Envia os processos criados em createStartingTimeTableCPU
    // para a CPU quando estiver no ciclo certo
    // A cada ciclo, esse método é chamado para que a fila referente
    // àquele ciclo seja processada e os processos que tenham que entrar
    // naquele momento sejam enviados a CPU
    if (cpu->cycles < MAX_STARTING_TIME + 1) {
        queue* queueNewProcesses = cpu->startingTimeTable[cpu->cycles];
        while (queueNewProcesses->size > 0) {
            process* proc = next(queueNewProcesses);
            proc->status = READY;
            insert(cpu->highPriorityQueue, proc);
        }
    }
}

void sendToLowPriorityQueue(cpu_t* cpu, process* proc) {
    // Envia processo `proc` para a fila de baixa prioridade
    printf("Enviando processo %d para a fila de baixa prioridade\n", proc->pid);
    proc->status = READY;
    insert(cpu->lowPriorityQueue, proc);
}

void sendProcessToIOQueue(cpu_t* cpu, process* proc) {
    // Envia processo `proc` para a fila de IO correspondente
    proc->status = BLOCKED;
    switch(proc->IOType) {
        case PRINTER:
            insert(cpu->PrinterQueue, proc);
            break;
        case MAGNETIC_TAPE:
            insert(cpu->MagneticTapeQueue, proc);
            break;
        case DISK:
            insert(cpu->DiskQueue, proc);
            break;
    }
}

void manageProcessRunning(cpu_t* cpu) {
    // Lida com o processo que estiver rodando na CPU nesse momento
    // Existem 4 possibilidades do que pode acontecer com um processo
    // que está na CPU:
    //      1. Ter chegado o tempo de início de IO
    //      2. O processo ter terminado (proc->elapsedTime == proc->duration)
    //      3. O quantum dele ter acabado (processo sofre preempção)
    //      4. Nenhum dos anteriores e o processo continua na CPU
    //
    // Se qualquer uma das condições 1-3 for satisfeita,
    // devemos resetar o quantum gasto pelo processo e alocar um novo processo na CPU
    process* currentProcess = cpu->executingProcess;

    if (hasReachedIOTime(currentProcess)) {
        printf("Enviando processo %d para a fila de %s\n", currentProcess->pid,
            getIoTypeAsString(currentProcess->IOType));

        resetQuantum(currentProcess);
        sendProcessToIOQueue(cpu, currentProcess);
        dispatchNextProcessToCPU(cpu);
    } else if (hasProcessFinished(currentProcess)) {
        currentProcess->status = FINISHED;
        printf(ANSI_COLOR_GREEN "Processo %d terminou, alocando CPU para outro processo\n"
               ANSI_COLOR_RESET, currentProcess->pid);

        resetQuantum(currentProcess);
        dispatchNextProcessToCPU(cpu);
    } else if (hasQuantumExpired(currentProcess, cpu->quantum)) {
        printf(ANSI_COLOR_RED "Processo %d sofreu preempção! " ANSI_COLOR_RESET,
            currentProcess->pid);
        resetQuantum(currentProcess);
        sendToLowPriorityQueue(cpu, currentProcess);
        dispatchNextProcessToCPU(cpu);
    } else {
        currentProcess->elapsedTimeCPU++;
        currentProcess->quantumCounter++;
    }
}

process* findNextProcess(cpu_t* cpu) {
    // Retorna qual é o próximo processo a ser alocado na CPU
    // Primeiro precisa checar se tem algum na fila de alta prioridade
    // e só depois checar se tem algum na fila de baixa prioridade
    process* proc = NULL;
    if (cpu->highPriorityQueue->size > 0)
        proc = next(cpu->highPriorityQueue);
    else if (cpu->lowPriorityQueue->size > 0)
        proc = next(cpu->lowPriorityQueue);
    return proc;
}

void dispatchNextProcessToCPU(cpu_t* cpu) {
    // Envia o próximo processo da fila para a CPU
    process* nextProcess = findNextProcess(cpu);
    if (nextProcess) {
        printf("Enviando processo %d para a CPU\n", nextProcess->pid);
        nextProcess->elapsedTimeCPU++;
        nextProcess->quantumCounter++;
        cpu->executingProcess = nextProcess;
        cpu->executingProcess->status = RUNNING;
    } else {
        cpu->executingProcess = NULL;
        printf("Nenhum processo foi alocado na CPU no ciclo %d\n", cpu->cycles);
    }
}

void handleIOProcesses(cpu_t* cpu) {
    // Lida com a fila de processos em IO
    handleIOQueue(cpu, cpu->PrinterQueue);
    handleIOQueue(cpu, cpu->MagneticTapeQueue);
    handleIOQueue(cpu, cpu->DiskQueue);
}

void handleIOQueue(cpu_t* cpu, queue* IOQueue) {
    // Lida com cada fila de IO
    // Se tiver alguém na fila,
    // precisamos checar se esse processo já gastou o tempo necessário de espera
    // Se tiver, precisamos mandá-lo se volta para a fila correspondente da CPU
    // (levando em conta as especificações do trabalho)
    if (IOQueue->size > 0) {
        process* IOProcess = peek(IOQueue);
        IOProcess->elapsedTimeIO++;
        if (hasIOFinished(IOProcess)) {
            next(IOQueue);
            if(hasProcessFinished(IOProcess)) {
                IOProcess->status = FINISHED;
                printf("Processo %d retornou de IO: %s, mas já terminou sua execução na CPU\n"
                       ANSI_COLOR_GREEN "Processo %d terminado\n" ANSI_COLOR_RESET,
                    IOProcess->pid, getIoTypeAsString(IOProcess->IOType), IOProcess->pid);
            } else {
                IOProcess->status = READY;
                insertAfterReturnFromIO(cpu, IOProcess);
            }
        }
    }
}

void roundRobin(cpu_t* cpu) {
    // Processo principal a ser executado
    // Primeiro lida com as filas de IO
    // E depois lida com o processo rodando, se houver algum
    // Ou coloca o próximo processo para rodar, se não houver algum
    handleIOProcesses(cpu);

    if(cpu->executingProcess) {
        manageProcessRunning(cpu);
    } else {
        dispatchNextProcessToCPU(cpu);
    }
}

void freeCPU(cpu_t* cpu) {
    // Libera os espaços de memória alocados
    free(cpu->executingProcess);
    free(cpu->highPriorityQueue);
    free(cpu->lowPriorityQueue);
    free(cpu->PrinterQueue);
    free(cpu->MagneticTapeQueue);
    free(cpu->DiskQueue);
    free(cpu);
}