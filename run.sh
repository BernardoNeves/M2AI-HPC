#!/bin/bash

shopt -s nullglob

SEQUENTIAL=0
PARALLEL=0
NUM_EXECUTIONS=10

files=(data/*.jss)

if (( ${#files[@]} == 0 )); then
    echo "No .jss files found in data/"
    exit 1
fi

for ((i = 1; i <= NUM_EXECUTIONS; i++)); do
    for file in "${files[@]}"; do
        base_name=$(basename "$file" .jss)

        # Sequential
        start_time=$(date +%s%N)
        bin/main -s -f "$file" -o "output/${base_name}_s.output"
        end_time=$(date +%s%N)
        elapsed_time=$((end_time - start_time))
        SEQUENTIAL=$((SEQUENTIAL + elapsed_time))

        # Parallel
        start_time=$(date +%s%N)
        bin/main -p -f "$file" -o "output/${base_name}_p.output"
        end_time=$(date +%s%N)
        elapsed_time=$((end_time - start_time))
        PARALLEL=$((PARALLEL + elapsed_time))
    done
done

count=${#files[@]}

SEQUENTIAL_MS=$((SEQUENTIAL / 1000000))
PARALLEL_MS=$((PARALLEL / 1000000))

AVG_SEQ=$((SEQUENTIAL_MS / count / NUM_EXECUTIONS))
AVG_PAR=$((PARALLEL_MS / count / NUM_EXECUTIONS))

# Calculate speedup as a floating-point value
speedup=$(awk "BEGIN { printf \"%.2fx\", $SEQUENTIAL_MS / $PARALLEL_MS }")

echo -e "=============================="
echo -e "Total files processed:       ${count}"
echo -e "Executions per file:        ${NUM_EXECUTIONS}"
echo -e "=============================="
echo -e "Total time for sequential: ${SEQUENTIAL_MS} ms"
echo -e "Average per file:          ${AVG_SEQ} ms"
echo -e "=============================="
echo -e "Total time for parallel:   ${PARALLEL_MS} ms"
echo -e "Average per file:          ${AVG_PAR} ms"
echo -e "=============================="
echo -e "Speedup (Sequential / Parallel): $speedup"
echo -e "=============================="

