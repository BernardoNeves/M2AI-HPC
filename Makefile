CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm -lpthread -ldl -lGL -lX11 -lrt
OPENMP_FLAGS = -fopenmp

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

BIN_DIR = bin
BINARY_NAME = main
BIN_PATH = $(BIN_DIR)/$(BINARY_NAME)

build:clean fmt
	@mkdir -p $(BIN_DIR)
	@$(CC) $(OPENMP_FLAGS) -o $(BIN_PATH) $(SOURCES) $(CFLAGS) $(LDFLAGS)

watch:
	@clear
	@printf "\033[1;33m[WATCH] Building...\033[0m\n"
	@$(MAKE) --no-print-directory build
	@printf "\033[1A\033[2K"
	@printf "\033[1;32m[WATCH] Build complete.\033[0m\n"
	@printf "\033[1;34m[WATCH] Watching for changes...\033[0m\n\n"
	@while true; do \
		inotifywait -qq -e modify $(SOURCES) $(HEADERS); \
		clear; \
		printf "\033[1;33m[WATCH] Change detected. Building...\033[0m\n"; \
		$(MAKE) --no-print-directory build; \
		printf "\033[1A\033[2K"; \
		printf "\033[1;32m[WATCH] Build complete.\033[0m\n"; \
		printf "\033[1;34m[WATCH] Watching for changes...\033[0m\n\n"; \
	done


clean:
	@rm -rf $(BIN_DIR)/*


fmt:
	@clang-format -i $(SOURCES) $(HEADERS)

.PHONY: build watch clean
