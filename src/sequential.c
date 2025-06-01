#include "sequential.h"

#include <stdio.h>
#include <time.h>

#include "utils.h"

void solve_sequential(const jobshop_t* jss) {
    printf("Running sequential Job Shop Scheduling solver\n");
    printf("Instance: %d jobs, %d machines\n", jss->num_jobs,
        jss->num_machines);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: Implement sequential JSS algorithm here

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = get_time_diff(start, end);

    printf("Sequential solver completed in %.6f seconds\n\n", elapsed);
}
