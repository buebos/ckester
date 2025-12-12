#ifndef __CKESTER_CORE_CONTRACTS_C__
#define __CKESTER_CORE_CONTRACTS_C__

#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * 5 MB
 */
const size_t CKESTER_DEFAULT_SIZE_PATH = 8 * 1000 * 1000 * 5;

/* Global jump buffer for assertion failures */
extern jmp_buf _ckester_jmp_env;

typedef enum Ckester_TestSuiteTag {
    CKESTER_FUNCTIONAL_UNIT,
    CKESTER_FUNCTIONAL_INTEGRATION,
    CKESTER_FUNCTIONAL_SYSTEM,
    CKESTER_PERFORMANCE_UNIT,
    CKESTER_PERFORMANCE_INTEGRATION,
    CKESTER_PERFORMANCE_SYSTEM,
    CKESTER_SECURITY_UNIT,
    CKESTER_SECURITY_INTEGRATION,
    CKESTER_SECURITY_SYSTEM,
} Ckester_TestSuiteTag;

/* Verbosity configuration */
typedef struct Ckester_OutputVerbosity {
    bool build;
    bool bin;
    bool src;
    bool suite;
    bool test;
    bool assertions;
    bool stdout;
    bool summary;
} Ckester_OutputVerbosity;

/* Test result tracking. */
typedef struct Ckester_TestResults {
    size_t total;
    size_t passed;
    size_t failed;
    const char* current_test;
    size_t current_assertions;
    Ckester_OutputVerbosity verbosity;
} Ckester_TestResults;

/* Test registration system. */
typedef struct Ckester_TestEntry {
    void (*func)(void);
    const char* name;
} Ckester_TestEntry;

typedef struct Ckester_TestRegistry {
    Ckester_TestEntry* tests;
    size_t count;
    size_t capacity;
} Ckester_TestRegistry;

typedef struct Ckester_CliContext {
    Ckester_OutputVerbosity verbosity;

    /**
     * A comma separated string of either file paths or directory paths to run
     * tests for. When empty, all C files and bash scripts on '$(pwd)/test/suite'
     * are run.
     */
    char* paths;

    char* test_dir;
    char* build_dir;
    char* c_compiler;
    char* filename_pattern;
} Ckester_CliContext;

#endif