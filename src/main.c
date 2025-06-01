#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "jobshop.h"
#include "parallel.h"
#include "sequential.h"
#include "utils.h"

static void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Job Shop Scheduling Problem Solver\n\n");
    printf("Options:\n");
    printf("  -s, --sequential    Run sequential JSS solver\n");
    printf("  -p, --parallel      Run parallel JSS solver (requires OpenMP)\n");
    printf("  -f, --file FILE     Input file containing JSS instance\n");
    printf("  -h, --help          Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s --sequential --file ft06.txt\n", program_name);
    printf("  %s -p -f jobshop_instance.txt\n", program_name);
    printf("\nInput format:\n");
    printf("  Line 1: <num_jobs> <num_machines>\n");
    printf("  Next lines: <machine> <time> pairs for each job\n");
    printf("  Comments start with # and are ignored\n");
}

int main(int argc, char** argv) {
    execution_mode_t mode = MODE_NONE;
    char* input_file = NULL;
    jobshop_t jss;

    static struct option long_options[] = {
        { "sequential", no_argument, 0, 's' },
        { "parallel", no_argument, 0, 'p' },
        { "file", required_argument, 0, 'f' },
        { "help", no_argument, 0, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "spf:h", long_options, NULL)) != -1) {
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

    print_jobshop_instance(&jss);

    switch (mode) {
    case MODE_SEQUENTIAL:
        solve_sequential(&jss);
        break;
    case MODE_PARALLEL:
        solve_parallel(&jss);
        break;
    default:
        fprintf(stderr, "Error: Invalid execution mode\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
