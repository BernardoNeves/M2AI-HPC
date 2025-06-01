#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int read_jobshop_from_file(const char* filename, jobshop_t* jss) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int line_num = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        if (sscanf(line, "%d %d", &jss->num_jobs, &jss->num_machines) == 2) {
            break;
        }
    }

    if (jss->num_jobs <= 0 || jss->num_machines <= 0 || jss->num_jobs > MAX_JOBS || jss->num_machines > MAX_MACHINES) {
        fprintf(stderr, "Error: Invalid number of jobs (%d) or machines (%d)\n",
            jss->num_jobs, jss->num_machines);
        fclose(file);
        return -1;
    }

    for (int job = 0; job < jss->num_jobs; job++) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "Error: Unexpected end of file at job %d\n", job);
            fclose(file);
            return -1;
        }

        char* token = strtok(line, " \t\n\r");
        for (int op = 0; op < jss->num_machines; op++) {
            if (!token) {
                fprintf(stderr,
                    "Error: Missing machine for job %d, operation %d\n",
                    job, op);
                fclose(file);
                return -1;
            }
            jss->machines[job][op] = atoi(token);

            token = strtok(NULL, " \t\n\r");
            if (!token) {
                fprintf(stderr,
                    "Error: Missing time for job %d, operation %d\n", job,
                    op);
                fclose(file);
                return -1;
            }
            jss->times[job][op] = atoi(token);
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(file);
    return 0;
}

void print_jobshop_instance(const jobshop_t* jss) {
    printf("Job Shop Scheduling Instance\n");
    printf("============================\n");
    printf("Number of jobs: %d\n", jss->num_jobs);
    printf("Number of machines: %d\n\n", jss->num_machines);

    printf("Job details (Machine Time pairs):\n");
    for (int job = 0; job < jss->num_jobs; job++) {
        printf("Job %d: ", job);
        for (int op = 0; op < jss->num_machines; op++) {
            printf("M%d(%d) ", jss->machines[job][op], jss->times[job][op]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Original format:\n");
    printf("%d %d\n", jss->num_jobs, jss->num_machines);
    for (int job = 0; job < jss->num_jobs; job++) {
        for (int op = 0; op < jss->num_machines; op++) {
            printf("%d %2d ", jss->machines[job][op], jss->times[job][op]);
        }
        printf("\n");
    }
    printf("\n");
}
