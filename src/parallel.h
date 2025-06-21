#ifndef PARALLEL_H
#define PARALLEL_H

#include "jobshop.h"

int solve_parallel(const jobshop_t* jss, jobshop_solution_t* solution, int num_threads);

#endif
