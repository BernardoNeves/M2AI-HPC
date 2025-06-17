#include "sequential.h"

#include <stdio.h>
#include <time.h>

#include "utils.h"

void solve_sequential(const jobshop_t* jss) {
    printf("Running sequential Job Shop Scheduling solver\n");
    printf("Instance: %d jobs, %d machines\n", jss->num_jobs,
        jss->num_machines);

    struct timespec start, end;
    //clock_gettime(CLOCK_MONOTONIC, &start);

    // Sequential JSS algorithm 
    // Arrays para manter disponibilidade
    int machine_available[MAX_MACHINES] = {0};
    int job_available[MAX_JOBS] = {0};

    // Tabela para guardar o tempo de início de cada operação [job][op]
    int start_times[MAX_JOBS][MAX_MACHINES];

    // Iterar por operações em sequência por job
    for (int job = 0; job < jss->num_jobs; job++) {
        for (int op = 0; op < jss->num_machines; op++) {
            int machine = jss->machines[job][op];
            int duration = jss->times[job][op];

            // Início é o máximo entre a disponibilidade do job e da máquina
            int start = (machine_available[machine] > job_available[job])
                            ? machine_available[machine]
                            : job_available[job];

            int end = start + duration;

            // Guardar para debug ou output
            start_times[job][op] = start;

            // Atualizar disponibilidade
            machine_available[machine] = end;
            job_available[job] = end;

            // Print opcional
            printf("Job %d - Op %d (Máquina %d): Start=%d End=%d\n",
                job, op, machine, start, end);
        }
    }
    //clock_gettime(CLOCK_MONOTONIC, &end);
    //double elapsed = get_time_diff(start, end);
    //printf("Sequential solver completed in %.6f seconds\n\n", elapsed);
}
