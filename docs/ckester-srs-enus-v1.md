# Ckester Software Requirements Specification (SRS)

**Version:** 1.0  
**Last Updated:** December 7, 2024  
**Status:** Draft

---

## 1. Introduction

### 1.1 Purpose

This document specifies the functional and non-functional requirements for Ckester, a lightweight C test runner designed to provide a developer experience similar to modern JavaScript/TypeScript and Python testing frameworks.

### 1.2 Scope

Ckester provides:

- A C library API for defining test suites and assertions
- A CLI tool for discovering, compiling, and executing tests
- Makefile integration for seamless workflow
- Cross-platform support with priority on MacOS, Linux, then Windows

### 1.3 Definitions

- **Test Suite:** Collection of related test cases identified by type and level
- **Test Case:** Individual test function within a suite
- **Type:** Test category (functional, performance, security)
- **Level:** Test scope (unit, integration, system)
- **Binary:** Compiled executable containing one or more test suites
- **Verbosity:** Output detail level controlling what information is displayed

---

## 2. System Overview

### 2.1 System Context

```
┌─────────────┐
│   Developer │
└──────┬──────┘
       │ writes tests using
       ↓
┌─────────────────┐     compiles with       ┌──────────┐
│  Ckester API    │────────────────────────→│ Compiler │
│  (Macros/Types) │                         └──────────┘
└─────────────────┘
       │ executes via
       ↓
┌─────────────────┐     discovers/runs     ┌──────────────┐
│  Ckester CLI    │←───────────────────────│  Makefile    │
│  (test runner)  │                        │  Integration │
└─────────────────┘                        └──────────────┘
       │
       ↓
┌─────────────────┐
│  Test Results   │
│  & Reports      │
└─────────────────┘
```

### 2.2 User Roles

- **Test Author:** Writes test suites using Ckester API
- **Test Runner:** Executes tests via CLI or make targets
- **CI/CD System:** Automated test execution in pipelines

---

## 3. Functional Requirements

### 3.1 Library API

#### 3.1.1 Test Suite Declaration

**REQ-API-001:** System shall provide `CKESTER_SUITE(name, type_level)` macro for declaring test suites.

**REQ-API-002:** Suite names must use `snake_case` convention.

**REQ-API-003:** System shall support three test types:

- `CKESTER_FUNCTIONAL`
- `CKESTER_PERFORMANCE`
- `CKESTER_SECURITY`

**REQ-API-004:** System shall support three test levels:

- `CKESTER_UNIT`
- `CKESTER_INTEGRATION`
- `CKESTER_SYSTEM`

**REQ-API-005:** Type and level shall be combined as single constant (e.g., `CKESTER_FUNCTIONAL_UNIT`).

#### 3.1.2 Test Case Declaration

**REQ-API-006:** System shall provide `CKESTER_TEST(name)` macro for declaring test cases within suites.

**REQ-API-007:** Test names must use `snake_case` convention.

**REQ-API-008:** Test ID shall be unique combination of name + type + level.

**REQ-API-009:** System shall reject duplicate test IDs with same name, type, and level.

#### 3.1.3 Assertion Macros

**REQ-API-010:** System shall provide basic assertion macros:

- `CKESTER_TRUE(condition, ...)`
- `CKESTER_FALSE(condition, ...)`

**REQ-API-011:** System shall provide equality assertion macros:

- `CKESTER_EQ(a, b, ...)`
- `CKESTER_NEQ(a, b, ...)`

**REQ-API-012:** System shall provide pointer assertion macros:

- `CKESTER_NULL(ptr, ...)`
- `CKESTER_NOT_NULL(ptr, ...)`

**REQ-API-013:** System shall provide string assertion macros:

- `CKESTER_STR_EQ(a, b, ...)`
- `CKESTER_STR_NEQ(a, b, ...)`

**REQ-API-014:** All assertion macros shall support optional formatted message using printf-style format strings.

**REQ-API-015:** Failed assertions shall report:

- File and line number
- Expected vs actual values (when applicable)
- Custom message (if provided)

### 3.2 Test Discovery

**REQ-DISC-001:** System shall recursively scan directories for test files.

**REQ-DISC-002:** Default test directory shall be `$PWD/test/suite`.

**REQ-DISC-003:** System shall support custom test directories via CLI arguments.

**REQ-DISC-004:** System shall match test files by pattern (default: `*_test.c`).

**REQ-DISC-005:** System shall support custom file patterns via configuration.

**REQ-DISC-006:** System shall exclude files in `test/util/` from discovery.

**REQ-DISC-007:** System shall filter tests by type (functional, performance, security).

**REQ-DISC-008:** System shall filter tests by level (unit, integration, system).

**REQ-DISC-009:** System shall support comma-separated list of files/directories.

**REQ-DISC-010:** System shall support escaped commas in file paths.

### 3.3 Test Compilation

**REQ-COMP-001:** Default build directory shall be `$PWD/build/ckester`.

**REQ-COMP-002:** System shall compile test files to executable binaries.

**REQ-COMP-003:** System shall check modification times before rebuilding.

**REQ-COMP-004:** System shall skip rebuild if source unchanged and binary exists.

**REQ-COMP-005:** System shall detect available C compiler (gcc, clang, msvc).

**REQ-COMP-006:** System shall support custom compiler via environment variable.

**REQ-COMP-007:** System shall batch multiple test files into single binary when beneficial.

**REQ-COMP-008:** System shall split into multiple binaries when size threshold exceeded.

**REQ-COMP-009:** System shall include Ckester library headers automatically.

**REQ-COMP-010:** Compiled binaries shall be named by content hash for caching.

### 3.4 Test Execution

**REQ-EXEC-001:** System shall execute compiled test binaries.

**REQ-EXEC-002:** System shall capture stdout and stderr from tests.

**REQ-EXEC-003:** System shall report test results in real-time.

**REQ-EXEC-004:** System shall continue execution after failed test (unless configured otherwise).

**REQ-EXEC-005:** System shall support bash script execution as test suites.

**REQ-EXEC-006:** System shall set appropriate exit codes:

- 0 for all tests passed
- Non-zero for any test failed

**REQ-EXEC-007:** System shall track execution time per test.

**REQ-EXEC-008:** System shall support test timeout configuration.

### 3.5 Reporting

**REQ-REPO-001:** System shall report test results with hierarchical output:

1. Source file
2. Test suite name
3. Test case name
4. Assertion results

**REQ-REPO-002:** System shall display summary statistics:

- Total builds performed
- Total binaries executed
- Total source files
- Total suites run
- Total tests executed
- Total assertions checked
- Pass/fail counts

**REQ-REPO-003:** System shall support configurable verbosity levels via `-v` flag:

- `build`: Show rebuild notifications
- `bin`: Show binary paths being executed
- `src`: Show source file paths
- `suite`: Show suite names
- `test`: Show test names
- `assert`: Show assertion details
- `stdout`: Show test stdout output
- `summary`: Show summary statistics

**REQ-REPO-004:** Default verbosity shall be: `build,bin,src,suite,test,stdout,summary`.

**REQ-REPO-005:** System shall provide `-a` or `--asserts` shorthand to add assertion verbosity.

**REQ-REPO-006:** System shall use color coding for output (pass=green, fail=red).

**REQ-REPO-007:** System shall indent output to reflect hierarchy levels.

### 3.6 CLI Interface

**REQ-CLI-001:** Running `ckester` with no arguments shall display help.

**REQ-CLI-002:** System shall support `ckester help` command.

**REQ-CLI-003:** System shall support `ckester run` command with syntax:

```
ckester run [paths] [type] [level] [options]
```

**REQ-CLI-004:** Paths shall be comma-separated files or directories.

**REQ-CLI-005:** Type and level filters shall be position-independent after paths.

**REQ-CLI-006:** System shall support verbosity options:

- `-v=<levels>` or `--verbosity=<levels>`
- `-a` or `--asserts`

**REQ-CLI-007:** Help output shall document all commands and options.

### 3.7 Makefile Integration

**REQ-MAKE-001:** System shall provide installable Makefile wrapper at `include/makefile`.

**REQ-MAKE-002:** Running `make` in Ckester directory shall build the CLI binary.

**REQ-MAKE-003:** Build process shall auto-inject include directive into `$PWD/makefile`.

**REQ-MAKE-004:** Include directive format shall be:

```makefile
# Ckester - This includes some testing targets for your makefile
include /path/to/ckester/include/makefile
```

**REQ-MAKE-005:** System shall not duplicate include directive if already present.

**REQ-MAKE-006:** Makefile wrapper shall provide targets prefixed with `ckester-*`:

| Target                    | Function                       |
| ------------------------- | ------------------------------ |
| `ckester-run`             | Run default tests (unit level) |
| `ckester-run-all`         | Run all test levels            |
| `ckester-run-unit`        | Run unit tests only            |
| `ckester-run-integration` | Run integration tests only     |
| `ckester-run-system`      | Run system tests only          |
| `ckester-run-single`      | Run specific test by name      |
| `ckester-clean`           | Clean build artifacts          |
| `ckester-help`            | Display help information       |

**REQ-MAKE-007:** All `ckester-*` targets shall depend on building Ckester binary.

**REQ-MAKE-008:** System shall support `FILE=<path>` variable for running specific files:

```bash
make ckester-run FILE=test/suite/functional/unit/array_test.c
```

**REQ-MAKE-009:** System shall support `NAME=<test_name>` variable for `ckester-run-single`:

```bash
make ckester-run-single NAME=array_push
```

**REQ-MAKE-010:** System shall support `M=<mode>` or `CKESTER_MODE=<mode>` variable for verbosity:

- `M=V` or `CKESTER_MODE=V`: Verbose mode (all verbosity enabled)
- `M=D` or `CKESTER_MODE=D`: Default mode
- `M=S` or `CKESTER_MODE=S`: Silent mode (minimal output)

**REQ-MAKE-011:** Example usage:

```bash
make ckester-run M=V
make ckester-run-unit CKESTER_MODE=S
make ckester-run FILE=test/suite/functional/unit/parser_test.c M=V
make ckester-run-single NAME=parser_basic CKESTER_MODE=D
```

**REQ-MAKE-012:** System shall support `CKESTER_BUILD_DIR=<path>` to override build directory.

**REQ-MAKE-013:** Makefile targets shall use `@` prefix to suppress command echo by default.

**REQ-MAKE-014:** `ckester-help` target shall display:

- Ckester CLI help output
- Makefile-specific usage examples

---

## 4. Non-Functional Requirements

### 4.1 Performance

**REQ-PERF-001:** Test discovery shall complete in under 1 second for 1000 files.

**REQ-PERF-002:** Compilation shall use incremental builds to minimize rebuild time.

**REQ-PERF-003:** System shall batch compilation when it improves performance.

**REQ-PERF-004:** Test execution shall have minimal overhead (< 10ms per test).

### 4.2 Portability

**REQ-PORT-001:** System shall support MacOS (primary platform).

**REQ-PORT-002:** System shall support Linux (secondary platform).

**REQ-PORT-003:** System shall support Windows (tertiary platform).

**REQ-PORT-004:** System shall use POSIX APIs when available.

**REQ-PORT-005:** System shall avoid Windows-specific APIs when possible.

**REQ-PORT-006:** System shall use `#ifdef` for platform-specific code.

**REQ-PORT-007:** System shall work with gcc, clang, and msvc compilers.

### 4.3 Usability

**REQ-USE-001:** Zero-configuration usage shall work for default directory structure.

**REQ-USE-002:** Error messages shall be clear and actionable.

**REQ-USE-003:** Help text shall include examples.

**REQ-USE-004:** API macros shall provide intuitive syntax.

**REQ-USE-005:** Makefile integration shall require no manual configuration.

### 4.4 Reliability

**REQ-REL-001:** Failed test shall not crash test runner.

**REQ-REL-002:** Invalid test file shall produce clear error message.

**REQ-REL-003:** System shall gracefully handle missing compiler.

**REQ-REL-004:** System shall handle permission errors appropriately.

### 4.5 Maintainability

**REQ-MAINT-001:** Code shall follow architecture specification.

**REQ-MAINT-002:** Code shall follow contributing guidelines.

**REQ-MAINT-003:** Public API shall be documented in header comments.

**REQ-MAINT-004:** Internal functions shall have block comment documentation.

### 4.6 Security

**REQ-SEC-001:** System shall validate all file paths to prevent path traversal.

**REQ-SEC-002:** System shall sanitize shell command arguments.

**REQ-SEC-003:** System shall not execute arbitrary code without explicit user intent.

**REQ-SEC-004:** Documentation shall warn about security limitations.

---

## 5. Configuration

### 5.1 Default Values

| Setting         | Default Value                             |
| --------------- | ----------------------------------------- |
| Test directory  | `$PWD/test/suite`                         |
| Build directory | `$PWD/build/ckester`                      |
| File pattern    | `*_test.c`                                |
| Verbosity       | `build,bin,src,suite,test,stdout,summary` |
| Test level      | `unit`                                    |
| Test type       | `functional`                              |

### 5.2 Configuration Methods

**REQ-CONF-001:** Configuration via CLI flags takes precedence.

**REQ-CONF-002:** Configuration via Makefile variables takes second precedence.

**REQ-CONF-003:** Environment variables take third precedence.

**REQ-CONF-004:** Default values are used when not specified.

---

## 6. Example Workflows

### 6.1 Basic Test Writing

```c
#include <ckester.h>

CKESTER_SUITE(array, CKESTER_FUNCTIONAL_UNIT) {
    CKESTER_TEST(push_adds_element) {
       int arr[10];
       int size = 0;
       push(arr, &size, 42);
       CKESTER_EQ(size, 1, "Size should be 1 after push");
       CKESTER_EQ(arr[0], 42, "Element should be 42");
    }
}
```

### 6.2 Running Tests via CLI

```bash
# Run all unit tests
ckester run test/suite/functional/unit

# Run specific file
ckester run test/suite/functional/unit/array_test.c

# Run with verbose output
ckester run -v=build,suite,test,assert,summary

# Run functional unit tests only
ckester run test/suite functional unit
```

### 6.3 Running Tests via Make

```bash
# Run unit tests (default)
make ckester-run

# Run all tests
make ckester-run-all

# Run with verbose output
make ckester-run M=V

# Run specific file
make ckester-run FILE=test/suite/functional/unit/array_test.c

# Run specific test
make ckester-run-single NAME=array_push

# Clean build artifacts
make ckester-clean
```

---

## 7. Out of Scope

The following are explicitly out of scope for version 1.0:

- **Code coverage reporting**
- **Memory leak detection**
- **Parallel test execution**
- **Test fixtures / setup-teardown**
- **Mocking framework**
- **Test result persistence**
- **CI/CD integration plugins**
- **GUI interface**
- **Remote test execution**
- **Test result comparison across runs**

These may be considered for future versions.

---

## 8. Acceptance Criteria

A release is considered complete when:

1. All functional requirements (REQ-API, REQ-DISC, REQ-COMP, REQ-EXEC, REQ-REPO, REQ-CLI, REQ-MAKE) are implemented
2. Test suite for Ckester itself passes (dogfooding)
3. Sample projects demonstrate all major features
4. Documentation is complete and accurate
5. System works on MacOS and Linux (Windows best-effort)
6. Zero-configuration workflow works for default structure
7. Makefile integration installs and works automatically

---

## 9. References

- Architecture Specification: `architecture.md`
- Contributing Guidelines: `contributing.md`
- API Documentation: `include/ckester.h`
- Code Guidelines: Internal document
