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

typedef struct {
    int machine_id;
    int start_time;
    int end_time;
} operation_schedule_t;

typedef struct {
    int num_jobs;
    int num_machines;
    int makespan;
    operation_schedule_t schedule[MAX_JOBS][MAX_MACHINES];
} jobshop_solution_t;

// dispatch_rule_t rules[] = {
//     SHORTEST_PROCESSING_TIME,
//     LONGEST_PROCESSING_TIME,
//     SHORTEST_REMAINING_TIME,
//     MOST_WORK_REMAINING,
//     FIRST_IN_FIRST_OUT,           // NEW - simple baseline
//     MOST_OPERATIONS_REMAINING,    // NEW - easy to add
//     RANDOM                        // NEW - good for testing
// };

typedef enum {
    FIRST_IN_FIRST_OUT,
    LONGEST_PROCESSING_TIME,
    MOST_OPERATIONS_REMAINING,
    MOST_WORK_REMAINING,
    RANDOM,
    SHORTEST_PROCESSING_TIME,
    SHORTEST_REMAINING_TIME,
    DISPATCH_RULE_COUNT
} dispatch_rule_t;

typedef struct {
    int job_ids[MAX_JOBS];
    int op_ids[MAX_JOBS];
    int machine_ids[MAX_JOBS];
    int processing_times[MAX_JOBS];
    int remaining_works[MAX_JOBS];
    double priorities[MAX_JOBS];
    int count;
} operation_batch_t;

typedef struct {
    int makespan;
    dispatch_rule_t rule_used;
    // Keep the full solution separate to avoid copying
} solution_summary_t;

#endif
