# Ckester Architecture Specification

## Overview

Ckester follows a layered domain architecture optimized for a C development tool. The system is organized around clear separation of concerns with strict dependency rules between layers.

## Directory Structure

```
ckester/
├── include/
│   └── ckester.h              # Public API header
├── src/
│   ├── components/            # Reusable utilities
│   ├── models/                # Domain data structures
│   ├── services/              # External integrations
│   ├── features/              # Business logic
│   └── core/                  # Cross-cutting concerns
├── test/
│   ├── suite/                 # Test files
│   │   ├── functional/
│   │   │   ├── unit/
│   │   │   ├── integration/
│   │   │   └── system/
│   │   ├── performance/
│   │   │   └── unit/
│   │   └── security/
│   │       └── unit/
│   └── util/                  # Test helpers
├── sample/                    # Example usage
├── vendor/                    # Git submodules
├── docs/                      # Documentation
├── scripts/                   # Build/automation scripts
├── makefile                   # Build configuration
└── README.md
```

## Layer Definitions

### Components (`src/components/`)

**Purpose:** Feature-agnostic, reusable building blocks used across the system.

**Examples:**

- `string_utils.c` - String manipulation helpers
- `array.c` - Dynamic array implementation
- `hashmap.c` - Hash table data structure
- `path.c` - Path manipulation utilities
- `color.c` - Terminal color output helpers

**Dependencies:** Other components only

**Naming:** `snake_case` for all files and functions

---

### Models (`src/models/`)

**Purpose:** Core domain data structures and their pure operations.

**Key Structures:**

- `TestSuite` - Represents a test suite with type and level
- `TestCase` - Individual test within a suite
- `TestResult` - Result of test execution
- `TestConfig` - Configuration for test runs
- `BuildArtifact` - Compiled binary metadata

**Example Files:**

- `test_suite.c` - Suite definition and operations
- `test_case.c` - Test case representation
- `test_result.c` - Result aggregation
- `config.c` - Configuration model

**Dependencies:** Components, other models

**Pattern:** Use Pascal case for struct names (e.g., `Ckester_TestSuite`), underscore prefix for private members in anonymous structs.

---

### Services (`src/services/`)

**Purpose:** Integration with external systems (filesystem, compiler, shell).

**Key Services:**

- `fs/` - Filesystem operations
  - `fs/scanner.c` - Recursive directory scanning
  - `fs/file.c` - File read/write operations
  - `fs/path.c` - Path resolution and validation
- `compiler/` - C compiler integration
  - `compiler/gcc.c` - GCC-specific compilation
  - `compiler/clang.c` - Clang-specific compilation
  - `compiler/detect.c` - Compiler detection
- `shell/` - Shell command execution
  - `shell/exec.c` - Process execution
  - `shell/script.c` - Bash script runner

**Dependencies:** Components only (no features or models)

**Pattern:** Each external system gets its own subdirectory

---

### Features (`src/features/`)

**Purpose:** User-facing business logic that orchestrates models and services.

**Key Features:**

- `discovery/` - Test file discovery
  - `discovery/scan.c` - Find test files recursively
  - `discovery/filter.c` - Filter by type/level/pattern
- `compilation/` - Test compilation
  - `compilation/build.c` - Compile test files
  - `compilation/cache.c` - Check for rebuild necessity
  - `compilation/optimize.c` - Batch compilation strategy
- `execution/` - Test execution
  - `execution/run.c` - Execute test binaries
  - `execution/capture.c` - Capture stdout/stderr
  - `execution/timeout.c` - Handle test timeouts
- `reporting/` - Test output and summaries
  - `reporting/format.c` - Format test results
  - `reporting/summary.c` - Aggregate statistics
  - `reporting/verbosity.c` - Handle verbosity levels
- `cli/` - Command-line interface
  - `cli/parse.c` - Parse CLI arguments
  - `cli/help.c` - Display help text
  - `cli/run.c` - Execute run command
- `library/` - The runtime testing library linked into test executables
  - `library/macros.c` - Assertion and test definition macros
  - `library/registry.c` - Test registration and management
  - `library/runner.c` - Test execution loop
  - `library/results.c` - Result tracking and aggregation

**Dependencies:** Models, services, components, other features

**Pattern:** Features map to user activities (run tests, discover tests, compile tests)

---

### Core (`src/core/`)

**Purpose:** Global foundational behaviors used across the system.

**Key Modules:**

- `error.c` - Global error handling
- `logger.c` - Logging infrastructure
- `memory.c` - Memory management utilities
- `constants.c` - System-wide constants

**Dependencies:** Components, services, models

**Pattern:** Used by features but not vice versa

---

## Public API (`include/`)

**Purpose:** Single header exposing the library interface.

**File:** `include/ckester.h`

**Contents:**

- Test suite macros (`CKESTER_SUITE`, `CKESTER_TEST`)
- Assertion macros (`CKESTER_EQ`, `CKESTER_TRUE`, etc.)
- Type and level enums
- No implementation details

**Pattern:** Include guards, minimal dependencies, user-facing only

---

## Test Structure (`test/`)

### Suite Organization

```
test/suite/{type}/{level}/*_test.c
```

**Types:**

- `functional/` - CKESTER_FUNCTIONAL
- `performance/` - CKESTER_PERFORMANCE
- `security/` - CKESTER_SECURITY

**Levels:**

- `unit/` - CKESTER_UNIT
- `integration/` - CKESTER_INTEGRATION
- `system/` - CKESTER_SYSTEM

### Test Utilities (`test/util/`)

Helper functions shared across tests (not compiled as tests themselves).

**Examples:**

- `test_helpers.c` - Common test setup/teardown
- `mock_fs.c` - Filesystem mocking utilities

---

## Dependency Rules

```
Components ←──────────────────────────┐
    ↑                                  │
Models ←───────┐                       │
    ↑          │                       │
Services ──────┴───────────────────────┤
    ↑                                  │
Core ──────────────────────────────────┤
    ↑                                  │
Features ←─────────────────────────────┘
    ↑
CLI Entry Point
```

**Rules:**

1. Components depend on nothing except other components
2. Models depend on components and other models
3. Services depend only on components (filesystem, compiler, shell have no business logic)
4. Core depends on components, services, and models
5. Features depend on everything except CLI
6. CLI depends only on features and components (for formatting)

---

## Build System

### Makefile Targets

```makefile
all:            # Build ckester binary
test:           # Run all tests
clean:          # Remove build artifacts
install:        # Install to system
sample:         # Build sample programs
```

### Build Directory

```
build/
├── ckester/           # Test binaries
│   └── {hash}.out     # Compiled test executable
└── bin/
    └── ckester        # CLI binary
```

**Pattern:** Test binaries named by content hash to enable caching

---

## Naming Conventions

### Files

- `snake_case.c` for all source files
- `snake_case.h` for all headers

### Structs

- Pascal case prefixed with namespace: `Ckester_TestSuite`
- Private members in anonymous struct with `_` label
- Private struct name pattern: `_Ckester_StructName`

### Functions

- `snake_case` for all functions
- Underscore prefix for private/internal functions: `_parse_internal`

### Macros

- `CAPITAL_SNAKE_CASE` for all macros
- Project prefix: `CKESTER_TEST`, `CKESTER_EQ`

### Variables

- `snake_case` for all variables
- Short names in tight loops acceptable: `i`, `j`, `len`

---

## Memory Management

### Principles

1. Prefer stack allocation when possible
2. Use `calloc` over `malloc` for zeroed memory
3. Functions that free memory should set pointer to `NULL` via pointer-to-pointer
4. Document ownership clearly in function comments
5. Provide cleanup functions for complex structures

### Pattern Example

```c
/* Allocates and initializes a test suite. Caller owns memory. */
Ckester_TestSuite *ckester_suite_create(const char *name);

/* Frees suite and sets pointer to NULL. */
void ckester_suite_free(Ckester_TestSuite **suite);
```

---

## Error Handling

### Strategy

1. Return error codes or NULL for recoverable errors
2. Use assertions for programmer errors
3. Log errors with context through `core/error.c`
4. Avoid silent failures

### Pattern

```c
typedef enum {
    CKESTER_OK = 0,
    CKESTER_ERR_FILE_NOT_FOUND,
    CKESTER_ERR_COMPILE_FAILED,
    CKESTER_ERR_INVALID_CONFIG
} Ckester_Error;
```

---

## Documentation

### File Headers

```c
/**
 * src/features/discovery/scan.c
 *
 * Recursively scans directories to discover test files matching
 * configured patterns. Filters results by type and level.
 *
 * Author: [name]
 * Last Modified: 2024-12-07
 */
```

### Function Documentation

```c
/**
 * Scans directory tree for test files.
 *
 * Recursively searches `root_dir` for files matching `pattern`,
 * filtering by `type` and `level` if specified. Results are
 * stored in dynamically allocated array.
 *
 * @param root_dir Base directory to scan
 * @param pattern Glob pattern for filenames (NULL for all .c files)
 * @param type Test type filter (0 for all types)
 * @param level Test level filter (0 for all levels)
 * @param out_count Pointer to store result count
 * @return Array of test file paths (caller owns memory)
 */
char **ckester_discover_tests(
    const char *root_dir,
    const char *pattern,
    int type,
    int level,
    size_t *out_count
);
```

---

## Example Usage

### Implementing a New Feature

When adding test filtering by name:

1. **Model:** Define filter criteria in `src/models/filter.c`
2. **Service:** No service needed (pure logic)
3. **Feature:** Implement in `src/features/discovery/filter.c`
4. **CLI:** Wire up in `src/features/cli/parse.c`

### Integrating External Tool

When adding support for a new compiler:

1. **Service:** Create `src/services/compiler/msvc.c`
2. **Core:** Update compiler detection in `src/services/compiler/detect.c`
3. **Feature:** Modify `src/features/compilation/build.c` to use new service
4. **Test:** Add compiler-specific tests in `test/suite/functional/unit/`

---

## Future Considerations

### Extensibility Points

- Plugin system for custom assertions
- Multiple output formats (JSON, XML, TAP)
- Remote test execution
- Coverage integration
- Memory leak detection

### Performance Optimizations

- Parallel compilation
- Parallel test execution
- Incremental builds
- Test result caching
