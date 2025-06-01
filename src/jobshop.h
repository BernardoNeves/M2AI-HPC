#ifndef JOBSHOP_H
#define JOBSHOP_H

#define MAX_JOBS 100
#define MAX_MACHINES 100
#define MAX_LINE_LENGTH 1024

typedef struct {
    int num_jobs;
    int num_machines;
    int machines[MAX_JOBS][MAX_MACHINES];
    int times[MAX_JOBS][MAX_MACHINES];
} jobshop_t;

typedef enum {
    MODE_NONE = 0,
    MODE_SEQUENTIAL = 1,
    MODE_PARALLEL = 2
} execution_mode_t;

#endif
