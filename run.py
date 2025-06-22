import argparse
import glob
import os
import subprocess
import sys
import time
from pathlib import Path
from typing import List, Dict
import matplotlib.pyplot as plt


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Benchmark script with thread scaling and plotting."
    )

    parser.add_argument(
        "-t",
        "--threads",
        type=str,
        help="Thread configuration: single number 4, comma-separated 2,4,8, or range 1:8",
        default="1,2,4,8,16,32",
    )

    parser.add_argument(
        "-n",
        "--num",
        type=int,
        default=10,
        help="Number of executions per file per configuration (default: 10)",
    )

    parser.add_argument(
        "files",
        nargs="*",
        help="File patterns or paths to .jss files. If not specified, defaults to data/*.jss",
    )

    args = parser.parse_args()

    if args.num <= 0:
        parser.error("Number of executions must be a positive integer")

    return args


def parse_thread_config(thread_str: str) -> List[int]:
    if not thread_str:
        return []

    threads = []

    if "," in thread_str:
        for part in thread_str.split(","):
            threads.extend(parse_thread_config(part.strip()))
    elif ":" in thread_str:
        start, end = map(int, thread_str.split(":"))
        threads.extend(range(start, end + 1))
    else:
        threads.append(int(thread_str))

    return sorted(list(set(threads)))


def resolve_file_patterns(patterns: List[str]) -> List[str]:
    if not patterns:
        print("No files specified, using default pattern: data/*.jss")
        patterns = ["data/*.jss"]

    resolved_files = []

    for pattern in patterns:
        expanded_files = glob.glob(pattern)

        if not expanded_files:
            print(
                f"Warning: No files found matching pattern: {pattern}", file=sys.stderr
            )
            continue

        for file_path in expanded_files:
            if not os.path.isfile(file_path):
                print(f"Warning: File not found: {file_path}", file=sys.stderr)
                continue

            if not file_path.endswith(".jss") and not file_path.endswith(".fss"):
                print(f"Warning: Skipping non-.jss file: {file_path}", file=sys.stderr)
                continue

            resolved_files.append(file_path)

    if not resolved_files:
        print("Error: No valid .jss files found", file=sys.stderr)
        sys.exit(1)

    resolved_files.sort()
    return resolved_files


def run_command(mode: str, file_path: str, thread_count: int) -> float:
    base_name = Path(file_path).stem

    if mode == "sequential":
        output_file = f"output/{base_name}_s.output"
        cmd = ["bin/main", "-s", "-f", file_path, "-o", output_file]
    else:
        output_file = f"output/{base_name}_p{thread_count}.output"
        cmd = [
            "bin/main",
            "-p",
            "-f",
            file_path,
            "-o",
            output_file,
            "-t",
            str(thread_count),
        ]

    try:
        start_time = time.time_ns()
        subprocess.run(cmd, check=True, capture_output=True)
        end_time = time.time_ns()

        return end_time - start_time
    except subprocess.CalledProcessError as e:
        thread_info = "" if mode == "sequential" else f" with {thread_count} threads"
        print(
            f"Error: {mode.capitalize()} execution{thread_info} failed for {file_path} with error:\n{e.stderr.decode().strip()}",
            file=sys.stderr,
        )
        sys.exit(1)


def run_benchmarks(
    files: List[str], args: argparse.Namespace, thread_counts: List[int]
) -> Dict:
    Path("output").mkdir(exist_ok=True)

    results = {}
    configs = []
    configs.append(("sequential", 0))
    for tc in thread_counts:
        configs.append(("parallel", tc))

    if not configs:
        print("Error: No configurations to run", file=sys.stderr)
        sys.exit(1)

    print("=" * 50)
    print("Benchmark Configuration")
    print("=" * 50)
    print(f"Files to process: {len(files)}")
    print(f"Executions per file per config: {args.num}")
    print(f"Configurations: {len(configs)}")
    print("=" * 50)

    print("Starting benchmark...")
    total_iterations = len(configs) * len(files) * args.num
    current_iteration = 0

    for config_idx, (mode, thread_count) in enumerate(configs):
        config_name = "seq" if mode == "sequential" else f"p{thread_count}"
        results[config_name] = {"total": 0}

        config_desc = (
            "Sequential"
            if mode == "sequential"
            else f"Parallel ({thread_count} threads)"
        )
        print(f"\n[{config_idx + 1}/{len(configs)}] Running {config_desc}")

        for f, file_path in enumerate(files):
            file_times = []

            for execution in range(args.num):
                print(
                    f"\rFile: {f + 1}/{len(files)} | Execution: {execution + 1}/{args.num} | Progress: ({current_iteration + 1}/{total_iterations})",
                    end="",
                    flush=True,
                )

                elapsed = run_command(mode, file_path, thread_count)
                file_times.append(elapsed)
                results[config_name]["total"] += elapsed
                current_iteration += 1

            results[config_name][file_path] = file_times

    print("\n")

    return results


def plot_results(results: Dict, files: List[str], args: argparse.Namespace):
    try:
        configs = list(results.keys())
        config_names = []
        avg_times = []
        total_times = []

        for config in configs:
            total_runs = len(files) * args.num
            total_time = results[config]["total"] / 1_000_000_000
            avg_time = total_time / total_runs

            config_names.append(config.upper())
            avg_times.append(avg_time)
            total_times.append(total_time)

        plt.figure(figsize=(12, 12))

        plt.subplot(3, 1, 1)
        bars1 = plt.bar(
            config_names, total_times, color="skyblue", edgecolor="navy", alpha=0.7
        )
        plt.title("Total Execution Time", fontsize=14, fontweight="bold")
        plt.xlabel("Configuration", fontsize=12)
        plt.ylabel("Time (seconds)", fontsize=12)
        plt.grid(axis="y", alpha=0.3)
        plt.axhline(
            y=results["seq"]["total"] / 1_000_000_000,
            color="blue",
            linestyle="--",
            alpha=0.5,
            label="Sequential Total",
        )
        plt.legend()

        for bar, time_val in zip(bars1, total_times):
            plt.text(
                bar.get_x() + bar.get_width() / 2,
                bar.get_height() + max(total_times) * 0.01,
                f"{time_val:.3f}",
                ha="center",
                va="bottom",
                fontsize=7,
            )
        plt.ylim(0, max(total_times) * 1.1)

        plt.subplot(3, 1, 2)
        bars2 = plt.bar(
            config_names, avg_times, color="lightcoral", edgecolor="darkred", alpha=0.7
        )
        plt.title("Average Execution Time per Run", fontsize=14, fontweight="bold")
        plt.xlabel("Configuration", fontsize=12)
        plt.ylabel("Time (seconds)", fontsize=12)
        plt.grid(axis="y", alpha=0.3)
        plt.axhline(
            y=results["seq"]["total"] / (len(files) * args.num * 1_000_000_000),
            color="red",
            linestyle="--",
            alpha=0.5,
            label="Sequential Average",
        )
        plt.legend()

        for bar, time_val in zip(bars2, avg_times):
            plt.text(
                bar.get_x() + bar.get_width() / 2,
                bar.get_height() + max(avg_times) * 0.01,
                f"{time_val:.3f}",
                ha="center",
                va="bottom",
                fontsize=7,
            )
        plt.ylim(0, max(avg_times) * 1.1)

        plt.subplot(3, 1, 3)
        speedups = [
            results["seq"]["total"] / results[config]["total"] for config in configs
        ]

        bars3 = plt.bar(
            config_names,
            speedups,
            color="lightgreen",
            edgecolor="darkgreen",
            alpha=0.7,
        )
        plt.title("Speedup", fontsize=14, fontweight="bold")
        plt.xlabel("Configuration", fontsize=12)
        plt.ylabel("Speedup Factor", fontsize=12)
        plt.grid(axis="y", alpha=0.3)
        plt.axhline(
            y=1, color="green", linestyle="--", alpha=0.5, label="Baseline (1x)"
        )
        plt.legend()
        plt.ylim(0, max(speedups) * 1.1)

        for bar, speedup in zip(bars3, speedups):
            plt.text(
                bar.get_x() + bar.get_width() / 2,
                bar.get_height() + max(speedups) * 0.01,
                f"{speedup:.2f}x",
                ha="center",
                va="bottom",
                fontsize=7,
            )

        plt.suptitle("Benchmark Results", fontsize=16, fontweight="bold")

        plt.tight_layout()
        plt.subplots_adjust(bottom=0.10, top=0.94)
        plt.figtext(
            0.5,
            0.02,
            f"Available processors: {os.cpu_count()}, Instances: {len(files)}, Executions per instance: {args.num}",
            wrap=True,
            horizontalalignment="center",
            fontsize=10,
            style="italic",
        )

        plt.savefig("benchmark_results.png", dpi=300, bbox_inches="tight")
        print("Plot saved as 'benchmark_results.png'")

        try:
            plt.show()
        except Exception as e:
            print("Could not display plot (running in headless environment):", e)

    except ImportError:
        print("Matplotlib not available. Install with: pip install matplotlib")
    except Exception as e:
        print(f"Error creating plot: {e}")


def main():
    try:
        args = parse_arguments()

        thread_counts = []
        if args.threads:
            thread_counts = parse_thread_config(args.threads)

        files = resolve_file_patterns(args.files)
        results = run_benchmarks(files, args, thread_counts)
        plot_results(results, files, args)
    except KeyboardInterrupt:
        print("\nBenchmark interrupted by user", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
