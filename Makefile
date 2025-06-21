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
	@$(MAKE) --no-print-directory run
	@printf "\033[1;34m[WATCH] Watching for changes...\033[0m\n\n"
	@while true; do \
		inotifywait -qq -e modify $(SOURCES) $(HEADERS); \
		clear; \
		printf "\033[1;33m[WATCH] Change detected. Building...\033[0m\n"; \
		$(MAKE) --no-print-directory build; \
		printf "\033[1A\033[2K"; \
		printf "\033[1;32m[WATCH] Build complete.\033[0m\n"; \
		$(MAKE) --no-print-directory run; \
		printf "\033[1;34m[WATCH] Watching for changes...\033[0m\n\n"; \
	done

run:
	@printf "\033[1;33m[RUN] Starting the program...\033[0m\n\n"
	@./run.sh
	@printf "\033[1;32m[RUN] Program finished.\033[0m\n"


clean:
	@rm -rf $(BIN_DIR)/*


fmt:
	@clang-format -i $(SOURCES) $(HEADERS)


help:
	@echo "Usage: make [target]"
	@echo "Available targets:"
	@echo "  build        - Build the binary"
	@echo "  run          - Run the built binary"
	@echo "  clean        - Clean build artifacts"
	@echo "  fmt          - Format source code"
	@echo "  watch        - Watch for changes and rebuild"
	@echo "  help         - Show this help message"
	@echo -e "\n After building run the binary with:"
	@echo "  ./$(BIN_PATH) [options]"


.PHONY: build watch clean fmt run help
