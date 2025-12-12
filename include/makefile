# Ckester Makefile Wrapper

# Default build directory
CKESTER_BUILD_DIR ?= build/ckester

# Default Ckester binary source
CKESTER_SRC ?= test/main.c
CKESTER_BIN ?= $(CKESTER_BUILD_DIR)/test_runner

# Ckester Include Path (assuming this file is at include/makefile, so root is ../)
CKESTER_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))..
CKESTER_INCLUDE ?= $(CKESTER_ROOT)/include

# Compiler flags
CFLAGS += -I$(CKESTER_INCLUDE)

# --- Targets ---

.PHONY: ckester-clean ckester-run ckester-run-all ckester-run-unit ckester-run-integration ckester-run-system ckester-run-single ckester-help

# Compile the test runner
$(CKESTER_BIN): $(CKESTER_SRC)
	@mkdir -p $(dir $@)
	@echo "[CKESTER] Compiling test runner..."
	@$(CC) $(CFLAGS) $< -o $@

# Wrapper for building
ckester-build: $(CKESTER_BIN)

# Run logic can be deferred to the binary itself if it supports CLI
# For now, we assume simple execution

ckester-run: ckester-build
	@echo "[CKESTER] Running tests..."
	@$(CKESTER_BIN) $(if $(VERBOSE),-v,)

ckester-clean:
	@rm -rf $(CKESTER_BUILD_DIR)
	@echo "[CKESTER] Cleaned build artifacts."

ckester-help:
	@echo "Ckester Makefile Help"
	@echo "  ckester-run      : Run tests"
	@echo "  ckester-clean    : Clean build"
