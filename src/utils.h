#ifndef UTILS_H
#define UTILS_H

#include <time.h>

#include "jobshop.h"

double get_time_diff(struct timespec start, struct timespec end);
int read_jobshop_from_file(const char* filename, jobshop_t* jss);
void print_jobshop_instance(const jobshop_t* jss);

#endif
