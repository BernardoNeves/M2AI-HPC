#include <stdio.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "parallel.h"
#include "utils.h"

void solve_parallel(const jobshop_t* jss) {
#ifdef _OPENMP
    printf("Running parallel Job Shop Scheduling solver\n");
    printf("Instance: %d jobs, %d machines\n", jss->num_jobs,
        jss->num_machines);
    printf("Number of threads: %d\n", omp_get_max_threads());

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: Implement parallel JSS algorithm here

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = get_time_diff(start, end);

    printf("Parallel solver completed in %.6f seconds\n\n", elapsed);
#else
    fprintf(
        stderr,
        "Error: OpenMP support not available. Falling back to sequential.\n");
#endif
}
