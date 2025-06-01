# HPC - High Performance Computing

> **a23494** Bernardo Neves<br>
> **a21190** Celestino Machado

## Build

To build the project:

```sh
make build
```

During development, you can automatically rebuild on file changes with:

```sh
make watch
```

## Format

To format the codebase, use the following command:

```sh
make fmt
```

## Run

Options for running the JSS solver:

| Short | Long            | Description                          |
|-------|-----------------|--------------------------------------|
| -s    | --sequential    | Run sequential JSS solver            |
| -p    | --parallel      | Run parallel JSS solver              |
| -f    | --file PATH     | Input file containing JSS instance   |
| -h    | --help          | Show help message                    |

Run the sequential JSS solver:
```sh
bin/main -s -f <path_to_file>
```
Run the parallel JSS solver:
```sh
bin/main -p -f <path_to_file>
```

