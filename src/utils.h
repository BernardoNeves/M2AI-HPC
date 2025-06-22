#ifndef UTILS_H
#define UTILS_H

#include <time.h>

#include "jobshop.h"

double get_time_diff(struct timespec start, struct timespec end);
int read_jobshop_from_file(const char* filename, jobshop_t* jss);
void print_jobshop_instance(const jobshop_t* jss);
int write_jobshop_solution_to_file(const char* filename, const jobshop_solution_t* solution);
void print_jobshop_solution(const jobshop_solution_t* solution);

int calculate_remaining_work(const jobshop_t* jss, int job_id, int op_idx);
int get_optimal_thread_count();

static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

#endif
