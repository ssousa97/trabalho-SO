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

    createStartingTimeTableCPU(cpu, processes);

    while(!allProcessFinished(processes)) {
        cpu->cycles++;
        printf(ANSI_COLOR_BLUE "Ciclo %d\n" ANSI_COLOR_RESET, cpu->cycles);

        sendNewProcessToCPU(cpu);

        roundRobin(cpu);

    }

    freeCPU(cpu);
    freeProcesses(processes);

    return 0;
}