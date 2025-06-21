#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H

#include "jobshop.h"

typedef enum {
    HEURISTIC_NONE = 0, // Original order
    HEURISTIC_SPT = 1, // Shortest Processing Time
    HEURISTIC_LPT = 2, // Longest Processing Time
    HEURISTIC_MWR = 3, // Most Work Remaining
    HEURISTIC_LWR = 4 // Least Work Remaining
} heuristic_t;

int solve_sequential(const jobshop_t* jss, jobshop_solution_t* solution);
int solve_sequential_with_heuristic(const jobshop_t* jss, jobshop_solution_t* solution, heuristic_t heuristic);
int solve_best_heuristic(const jobshop_t* jss, jobshop_solution_t* solution);

#endif
