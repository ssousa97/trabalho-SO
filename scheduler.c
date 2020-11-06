#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "cpu.h"
#include "util.h"

int main() {

    srand(time(0));

    cpu_t* cpu = initCPU();
    process** processes = initRandomProcesses();

    while(!allProcessFinished(processes)) {
        cpu->cycles++;
        printf(ANSI_COLOR_BLUE "Ciclo %d\n" ANSI_COLOR_RESET, cpu->cycles);

        addReadyProcessesToCPU(cpu, processes);

        roundRobin(cpu);

        if (cpu->cycles > 40) break;
    }

    // freeCPU(cpu);
    // freeProcesses(processes);

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