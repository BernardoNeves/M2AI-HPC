#include "parallel.h"
#include "jobshop.h"
#include "utils.h"
#include <limits.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>

static int solve_with_dispatch_rule_parallel(const jobshop_t* jss, jobshop_solution_t* solution, dispatch_rule_t rule) {
    int machine_completion_time[MAX_MACHINES] = { 0 };
    int job_next_operation[MAX_JOBS] = { 0 };
    int operations_completed = 0;
    int total_operations = jss->num_jobs * jss->num_machines;

    solution->num_jobs = jss->num_jobs;
    solution->num_machines = jss->num_machines;

    while (operations_completed < total_operations) {
        // Find all ready operations using struct-of-arrays
        operation_batch_t ready_ops = { .count = 0 };

        for (int job_id = 0; job_id < jss->num_jobs; job_id++) {
            int op_idx = job_next_operation[job_id];
            if (op_idx < jss->num_machines) {
                int idx = ready_ops.count;
                ready_ops.job_ids[idx] = job_id;
                ready_ops.op_ids[idx] = op_idx;
                ready_ops.machine_ids[idx] = jss->machines[job_id][op_idx];
                ready_ops.processing_times[idx] = jss->times[job_id][op_idx];
                ready_ops.remaining_works[idx] = calculate_remaining_work(jss, job_id, op_idx);

                // Calculate priority based on dispatch rule
                switch (rule) {
                case SHORTEST_PROCESSING_TIME:
                    ready_ops.priorities[idx] = ready_ops.processing_times[idx];
                    break;
                case LONGEST_PROCESSING_TIME:
                    ready_ops.priorities[idx] = -ready_ops.processing_times[idx];
                    break;
                case SHORTEST_REMAINING_TIME:
                    ready_ops.priorities[idx] = ready_ops.remaining_works[idx];
                    break;
                case MOST_WORK_REMAINING:
                    ready_ops.priorities[idx] = -ready_ops.remaining_works[idx];
                    break;
                case FIRST_IN_FIRST_OUT:
                    ready_ops.priorities[idx] = job_id;
                    break;
                case MOST_OPERATIONS_REMAINING:
                    ready_ops.priorities[idx] = -(jss->num_machines - op_idx);
                    break;
                case RANDOM:
                    ready_ops.priorities[idx] = rand() % 1000;
                    break;
                default:
                    ready_ops.priorities[idx] = job_id; // FIFO as fallback
                }
                ready_ops.count++;
            }
        }

        // Find operation with best priority (minimum value)
        int best_idx = 0;
        for (int i = 1; i < ready_ops.count; i++) {
            if (ready_ops.priorities[i] < ready_ops.priorities[best_idx]) {
                best_idx = i;
            }
        }

        // Schedule the highest priority operation
        int job_id = ready_ops.job_ids[best_idx];
        int op_idx = ready_ops.op_ids[best_idx];
        int machine_id = ready_ops.machine_ids[best_idx];
        int processing_time = ready_ops.processing_times[best_idx];

        // Calculate start time
        int job_completion_time = 0;
        if (op_idx > 0) {
            job_completion_time = solution->schedule[job_id][op_idx - 1].end_time;
        }

        int start_time = max(machine_completion_time[machine_id], job_completion_time);

        // Store in solution
        solution->schedule[job_id][op_idx].machine_id = machine_id;
        solution->schedule[job_id][op_idx].start_time = start_time;
        solution->schedule[job_id][op_idx].end_time = start_time + processing_time;

        // Update completion times
        machine_completion_time[machine_id] = start_time + processing_time;
        job_next_operation[job_id]++;
        operations_completed++;
    }

    // Calculate makespan
    int makespan = 0;
    for (int job_id = 0; job_id < jss->num_jobs; job_id++) {
        int job_end = solution->schedule[job_id][jss->num_machines - 1].end_time;
        if (job_end > makespan) {
            makespan = job_end;
        }
    }

    solution->makespan = makespan;
    return makespan;
}

int solve_multi_pass_parallel(const jobshop_t* jss, jobshop_solution_t* solution, int num_threads) {
    omp_set_num_threads(num_threads);

    // Use solution summaries to track results
    solution_summary_t results[DISPATCH_RULE_COUNT];
    jobshop_solution_t temp_solutions[DISPATCH_RULE_COUNT];

    // Try different dispatch rules
    dispatch_rule_t rules[] = {
        SHORTEST_PROCESSING_TIME,
        LONGEST_PROCESSING_TIME,
        SHORTEST_REMAINING_TIME,
        MOST_WORK_REMAINING,
        FIRST_IN_FIRST_OUT,
        MOST_OPERATIONS_REMAINING,
        RANDOM
    };

    int num_rules = sizeof(rules) / sizeof(rules[0]);

    // #pragma omp parallel for
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < num_rules; i++) {
        int makespan = solve_with_dispatch_rule_parallel(jss, &temp_solutions[i], rules[i]);

        results[i].makespan = makespan;
        results[i].rule_used = rules[i];
    }

    // Find best result
    int best_idx = 0;
    for (int i = 1; i < num_rules; i++) {
        if (results[i].makespan < results[best_idx].makespan) {
            best_idx = i;
        }
    }

    // Copy best solution
    memcpy(solution, &temp_solutions[best_idx], sizeof(jobshop_solution_t));
    return results[best_idx].makespan;
}

int solve_parallel(const jobshop_t* jss, jobshop_solution_t* solution, int num_threads) {
    return solve_multi_pass_parallel(jss, solution, num_threads);
}
