# Ckester Contributing Guidelines

## Quick Start

1. **Clone with submodules:** `git clone --recursive`
2. **Build:** `make all`
3. **Test:** `make test`
4. **Run sample:** `make sample && ./build/sample/basic`

---

## Code Style

### Formatting

Use VSCode C/C++ extension with this configuration:

```json
{
  "BasedOnStyle": "Google",
  "IndentWidth": 4,
  "ColumnLimit": 0
}
```

### Naming

- **Files:** `snake_case.c`
- **Functions:** `snake_case()`, prefix with `_` for private
- **Structs:** `Ckester_PascalCase`
- **Macros:** `CKESTER_CAPITAL_SNAKE`
- **Variables:** `snake_case`

### Memory

- Prefer stack allocation
- Use `calloc` for heap allocation (zeroed memory)
- Functions that free should take `**ptr` and set to `NULL`
- Document ownership in comments

```c
/* Creates suite. Caller owns memory. */
Ckester_TestSuite *ckester_suite_create(const char *name);

/* Frees suite and nullifies pointer. */
void ckester_suite_free(Ckester_TestSuite **suite);
```

---

## Architecture Layers

**Strict dependency rules:**

```
Components (utilities, no dependencies)
    ↓
Models (data structures, depend on components)
    ↓
Services (external integrations, depend on components only)
    ↓
Core (global concerns, depend on components/services/models)
    ↓
Features (business logic, depend on all above)
    ↓
CLI (entry point, depends on features/components)
```

### When to Use Each Layer

**Components** (`src/components/`):

- String utils, collections, path helpers
- No external dependencies, pure utilities
- Example: `hashmap.c`, `string_utils.c`

**Models** (`src/models/`):

- Data structures: `TestSuite`, `TestResult`, `BuildArtifact`
- Pure operations on data
- Example: `test_suite.c`, `config.c`

**Services** (`src/services/`):

- Filesystem, compiler, shell integration
- One subdirectory per external system
- Example: `fs/scanner.c`, `compiler/gcc.c`

**Core** (`src/core/`):

- Error handling, logging, memory utilities
- Used everywhere, changes rarely
- Example: `error.c`, `logger.c`

**Features** (`src/features/`):

- Discovery, compilation, execution, reporting
- Orchestrates models and services
- Example: `discovery/scan.c`, `execution/run.c`

---

## Comments

### Block Comments

Always use block comments, even for single lines:

```c
/* Single line block comment. */

/**
 * Multi-line block comment for functions.
 * Document parameters and return values.
 */
```

### File Headers

```c
/**
 * src/features/discovery/scan.c
 *
 * Discovers test files by recursively scanning directories.
 * Filters by type, level, and pattern matching.
 *
 * Author: Jane Doe
 * Last Modified: 2024-12-07
 */
```

### Function Documentation

```c
/**
 * Compiles test file to binary.
 *
 * Uses detected compiler to build `source_file` into executable
 * in build directory. Checks modification times to skip rebuild
 * if binary is up-to-date.
 *
 * @param source_file Path to .c test file
 * @param build_dir Directory for output binary
 * @param force_rebuild Ignore cache, always rebuild
 * @return Path to compiled binary or NULL on error
 */
char *ckester_compile_test(
    const char *source_file,
    const char *build_dir,
    int force_rebuild
);
```

### Inline Comments

Focus on **why**, not **what**:

```c
/* Use calloc to ensure test result fields are zeroed. */
result = calloc(1, sizeof(Ckester_TestResult));

/* Skip files in util/ to avoid compiling helpers as tests. */
if (strstr(path, "/util/") != NULL) {
    continue;
}
```

### Section Markers

Mark major code sections:

```c
/* ==================== Discovery ==================== */

/* ==================== Compilation ==================== */

/* ==================== Execution ==================== */
```

---

## Adding Features

### 1. Identify Layer

- **Utility function?** → Components
- **Data structure?** → Models
- **External system?** → Services
- **User workflow?** → Features

### 2. Create Module

Place in appropriate directory:

```bash
src/features/coverage/
├── collect.c        # Collect coverage data
├── report.c         # Generate coverage report
└── integrate.c      # Integrate with gcov/llvm-cov
```

### 3. Update Public API

If user-facing, add to `include/ckester.h`:

```c
/* Coverage macros (if adding coverage feature). */
#define CKESTER_COVERAGE_START() ...
#define CKESTER_COVERAGE_END() ...
```

### 4. Write Tests

Add tests in appropriate category:

```bash
test/suite/functional/unit/coverage_collect_test.c
test/suite/functional/integration/coverage_report_test.c
```

### 5. Document

Add to `docs/` and update README if needed.

---

## Common Patterns

### Error Handling

```c
typedef enum {
    CKESTER_OK = 0,
    CKESTER_ERR_FILE_NOT_FOUND,
    CKESTER_ERR_COMPILE_FAILED
} Ckester_Error;

Ckester_Error result = ckester_do_thing();
if (result != CKESTER_OK) {
    /* Handle error. */
    return result;
}
```

### Dynamic Arrays

Use component utilities:

```c
#include "components/array.h"

char **files = NULL;
size_t count = 0;

ckester_array_push(&files, &count, "test1.c");
ckester_array_push(&files, &count, "test2.c");

/* Use files... */

ckester_array_free(&files, count);
```

### Private Struct Members

```c
typedef struct {
    char *name;           /* Public field. */
    int type;             /* Public field. */

    /* Private fields. */
    struct _Ckester_TestSuite {
        void **internal_state;
        size_t state_count;
    } _;
} Ckester_TestSuite;

/* Access: suite._._internal_state */
```

---

## Testing

### Writing Tests

Use Ckester's own API:

```c
#include <ckester.h>

CKESTER_SUITE(scanner, CKESTER_FUNCTIONAL_UNIT) {
    CKESTER_TEST(finds_c_files) {
        char **files = ckester_scan_directory("test/fixture");
        CKESTER_NOT_NULL(files, "Should find files");
        CKESTER_TRUE(file_count > 0, "Should find at least one");
    }

    CKESTER_TEST(filters_by_pattern) {
        char **files = ckester_scan_directory_pattern("test/", "*_test.c");
        CKESTER_TRUE(all_match_pattern(files), "All files should match");
    }
}
```

### Test Organization

- **Unit:** Test single functions in isolation
- **Integration:** Test multiple components together
- **System:** Test complete workflows

Place in: `test/suite/{type}/{level}/`

### Test Utilities

Shared helpers go in `test/util/`:

```c
/* test/util/test_helpers.c */

char *create_temp_test_file(const char *content) {
    /* Create temporary file for testing. */
}

void cleanup_temp_files(void) {
    /* Remove all temporary test files. */
}
```

---

## Building and Running

### Makefile Targets

```bash
make all        # Build ckester binary
make test       # Run all tests
make clean      # Remove build artifacts
make install    # Install to /usr/local/bin
make sample     # Build sample programs
```

### Running Specific Tests

```bash
# Run all unit tests
./build/bin/ckester run test/suite/functional/unit

# Run specific test file
./build/bin/ckester run test/suite/functional/unit/scanner_test.c

# Run with verbosity
./build/bin/ckester run -v=build,src,suite,test,assert,summary

# Run only functional tests
./build/bin/ckester run test/suite functional

# Run only unit-level tests
./build/bin/ckester run test/suite unit
```

---

## Portability

### Platform Support

Priority order: MacOS → Linux → Windows

### Guidelines

- Use POSIX APIs when available
- Avoid Windows-specific APIs (`FindFirstFile`, registry, etc.)
- Use `#ifdef` for platform-specific code:

```c
#ifdef _WIN32
    /* Windows-specific implementation. */
#else
    /* POSIX implementation. */
#endif
```

### Cross-Platform Testing

Test on multiple platforms before submitting:

```bash
# Linux
make clean && make test

# MacOS
make clean && make test

# Windows (MinGW)
mingw32-make clean && mingw32-make test
```

---

## Dependencies

### Managing Submodules

Add new dependencies as git submodules:

```bash
git submodule add https://github.com/user/library vendor/library
git submodule update --init --recursive
```

Update existing:

```bash
git submodule update --remote vendor/library
```

### Vendor Directory

```
vendor/
├── some-library/      # External dependency
└── another-lib/       # Another dependency
```

---

## Pull Request Checklist

- [ ] Code follows style guidelines (Google style, 4-space indent)
- [ ] Functions documented with block comments
- [ ] Memory properly managed (no leaks)
- [ ] Layer dependencies respected
- [ ] Tests written and passing
- [ ] Works on MacOS/Linux (Windows if possible)
- [ ] README updated if adding user-facing features
- [ ] No compiler warnings

---

## Performance Considerations

### Compilation

- Batch compilations when possible
- Cache build artifacts by content hash
- Skip rebuild if source unchanged

### Execution

- Stream output rather than buffering
- Release memory promptly
- Avoid unnecessary allocations in loops

### Pattern

```c
/* Good: Allocate once. */
char *buffer = calloc(1024, 1);
for (int i = 0; i < count; i++) {
    format_into_buffer(buffer, items[i]);
    process(buffer);
}
free(buffer);

/* Bad: Allocate in loop. */
for (int i = 0; i < count; i++) {
    char *buffer = calloc(1024, 1);
    format_into_buffer(buffer, items[i]);
    process(buffer);
    free(buffer);
}
```

---

## Security Notes

**WARNING:** Ckester is not hardened for security.

When contributing, avoid:

- Arbitrary command execution without validation
- Buffer overflows (use bounded string operations)
- Path traversal vulnerabilities
- Injection attacks in compiler/shell commands

If adding security features, document thoroughly.

---

## Getting Help

- **Architecture questions:** See [Architecture Spec](ARCHITECTURE.md)
- **API questions:** See `include/ckester.h` comments
- **Bug reports:** Open GitHub issue with reproduction
- **Feature requests:** Open GitHub issue with use case

---

## Example Contribution Flow

### Adding JSON Test Results Output

1. **Plan the layers:**

   - Component: JSON serialization utility (`src/components/json.c`)
   - Model: No changes needed (`TestResult` already exists)
   - Service: File writing service already exists
   - Feature: New reporter (`src/features/reporting/json.c`)
   - CLI: Add `--format=json` option

2. **Implement bottom-up:**

```c
/* src/components/json.c */
char *json_serialize_object(/* ... */);

/* src/features/reporting/json.c */
void ckester_report_json(
    Ckester_TestResult *results,
    size_t count,
    const char *output_file
) {
    char *json = json_serialize_test_results(results, count);
    ckester_write_file(output_file, json);
    free(json);
}

/* src/features/cli/parse.c */
if (strcmp(format, "json") == 0) {
    config.reporter = CKESTER_REPORTER_JSON;
}
```

3. **Add tests:**

```c
/* test/suite/functional/unit/json_reporter_test.c */
CKESTER_SUITE(json_reporter, CKESTER_FUNCTIONAL_UNIT) {
    CKESTER_TEST(formats_results) {
        /* Create test results. */
        /* Generate JSON. */
        /* Verify JSON structure. */
    }
}
```

4. **Document:**

   - Add to README CLI options section
   - Update `docs/OUTPUT_FORMATS.md`
   - Add example to `sample/`

5. **Submit PR** with description of use case and design decisions.
