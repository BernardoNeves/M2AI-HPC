#define _POSIX_C_SOURCE 200809L
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "jobshop.h"
#include "parallel.h"
#include "sequential.h"
#include "utils.h"

#define DEFAULT_OUTPUT_DIR "./output"

static void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Job Shop Scheduling Problem Solver\n\n");
    printf("Options:\n");
    printf("  -s, --sequential    Run sequential JSS solver\n");
    printf("  -p, --parallel      Run parallel JSS solver (requires OpenMP)\n");
    printf("  -f, --file FILE     Input file containing JSS instance\n");
    printf("  -o, --output [FILE] Output file for results (optional)\n");
    printf("  -t, --threads N     Number of threads to use (default: auto)\n");
    printf("  -h, --help          Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s --sequential --file ft06.txt\n", program_name);
    printf("  %s -p -f jobshop_instance.txt\n", program_name);
    printf("  %s -p -f jobshop_instance.txt -o results.output\n", program_name);
    printf("\nInput format:\n");
    printf("  Line 1: <num_jobs> <num_machines>\n");
    printf("  Next lines: <machine> <time> pairs for each job\n");
    printf("  Comments start with # and are ignored\n");
}

int main(int argc, char** argv) {
    execution_mode_t mode = MODE_NONE;
    char* input_file = NULL;
    char* output_file = NULL;
    jobshop_t jss;
    jobshop_solution_t solution;
    int num_threads = get_optimal_thread_count();

    static struct option long_options[] = {
        { "sequential", no_argument, 0, 's' },
        { "parallel", no_argument, 0, 'p' },
        { "file", required_argument, 0, 'f' },
        { "output", required_argument, 0, 'o' },
        { "threads", required_argument, 0, 't' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "spf:o:t:h", long_options, NULL)) != -1) {
        switch (opt) {
        case 's':
            if (mode != MODE_NONE) {
                fprintf(stderr,
                    "Error: Cannot specify both sequential and "
                    "parallel modes\n");
                return EXIT_FAILURE;
            }
            mode = MODE_SEQUENTIAL;
            break;
        case 'p':
            if (mode != MODE_NONE) {
                fprintf(stderr,
                    "Error: Cannot specify both sequential and "
                    "parallel modes\n");
                return EXIT_FAILURE;
            }
            mode = MODE_PARALLEL;
            break;
        case 'f':
            input_file = optarg;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 't': {
            char* endptr;
            num_threads = strtol(optarg, &endptr, 10);
            if (*endptr != '\0' || num_threads <= 0) {
                fprintf(stderr, "Error: Invalid number of threads '%s'\n",
                    optarg);
                return EXIT_FAILURE;
            }
            break;
        }
        case 'h':
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (mode == MODE_NONE) {
        fprintf(stderr,
            "Error: You must specify either sequential (-s) or "
            "parallel (-p) mode\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!input_file) {
        fprintf(stderr,
            "Error: You must specify an input file with -f/--file\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (read_jobshop_from_file(input_file, &jss) < 0) {
        return EXIT_FAILURE;
    }

    // print_jobshop_instance(&jss);

    solution.num_jobs = jss.num_jobs;
    solution.num_machines = jss.num_machines;
    solution.makespan = 0;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    switch (mode) {
    case MODE_SEQUENTIAL:
        solve_sequential(&jss, &solution);
        break;
    case MODE_PARALLEL:
        solve_parallel(&jss, &solution, num_threads);
        break;
    default:
        fprintf(stderr, "Error: Invalid execution mode\n");
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = get_time_diff(start_time, end_time);
    printf("Solved in:\t %.6fs\n", elapsed);
    printf("Makespan:\t %d\n", solution.makespan);

    // print_jobshop_solution(&solution);
    if (output_file) {
        if (write_jobshop_solution_to_file(output_file, &solution) < 0) {
            fprintf(stderr, "Error: Failed to write solution to file %s\n",
                output_file);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
