#ifndef __CKESTER_FEATURES_LIBRARY_MACROS_C__
#define __CKESTER_FEATURES_LIBRARY_MACROS_C__

/* Definitions to avoid multiple symbol collisions during batch compilation */
#if defined(__GNUC__) || defined(__clang__)
#define CKESTER_WEAK __attribute__((weak))
#elif defined(_MSC_VER)
#define CKESTER_WEAK __declspec(selectany)
#else
#define CKESTER_WEAK
#endif

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/contracts.c"
#include "../../services/logger.c"

/* Global jump buffer definition */
CKESTER_WEAK jmp_buf _ckester_jmp_env;

/* Global test results with default verbosity */
CKESTER_WEAK Ckester_TestResults _ckester_results = {
    0,
    0,
    0,
    NULL,
    0,
    /* Default verbosity: build, bin, src, suite, test, show_stdout, summary
       (assertions=false) */
    {true, true, true, true, true, false, true, true}};

/* Helper to enable full verbosity (debug mode) */
CKESTER_WEAK void ckester_results_enable_all_verbosity(void) {
    _ckester_results.verbosity.build = true;
    _ckester_results.verbosity.bin = true;
    _ckester_results.verbosity.src = true;
    _ckester_results.verbosity.suite = true;
    _ckester_results.verbosity.test = true;
    _ckester_results.verbosity.assertions = true;
    _ckester_results.verbosity.stdout = true;
    _ckester_results.verbosity.summary = true;
}

/* Global registry instance */
CKESTER_WEAK Ckester_TestRegistry _ckester_registry = {NULL, 0, 0};

/* Register a test function with its name */
CKESTER_WEAK void _ckester_register_test(void (*test)(void), const char* name) {
    if (_ckester_registry.count >= _ckester_registry.capacity) {
        _ckester_registry.capacity =
            _ckester_registry.capacity == 0 ? 8 : _ckester_registry.capacity * 2;
        _ckester_registry.tests =
            realloc(_ckester_registry.tests,
                    _ckester_registry.capacity * sizeof(Ckester_TestEntry));
    }
    _ckester_registry.tests[_ckester_registry.count].func = test;
    _ckester_registry.tests[_ckester_registry.count].name = name;
    _ckester_registry.count++;
}

/* Cleanup registry memory */
CKESTER_WEAK void _ckester_free_registry(void) {
    if (_ckester_registry.tests != NULL) {
        free(_ckester_registry.tests);
        _ckester_registry.tests = NULL;
    }
    _ckester_registry.count = 0;
    _ckester_registry.capacity = 0;
}

/* State for test execution loop */
CKESTER_WEAK struct {
    size_t failures_at_start;
    bool active;
} _ckester_current_test_state;

CKESTER_WEAK void _ckester_test_begin(const char* name) {
    _ckester_results.current_test = name;
    _ckester_current_test_state.failures_at_start = _ckester_results.failed;
    _ckester_current_test_state.active = true;
    _ckester_results.current_assertions = 0;
}

CKESTER_WEAK int _ckester_test_should_run(void) {
    return _ckester_current_test_state.active;
}

CKESTER_WEAK void _ckester_test_end(void) {
    _ckester_current_test_state.active = false;
    
    bool failed = _ckester_results.failed > _ckester_current_test_state.failures_at_start;
    
    if (_ckester_results.verbosity.test) {
        ckester_log_test_result(_ckester_results.current_test, failed);
    }
}

extern Ckester_TestRegistry _ckester_registry;
extern Ckester_TestResults _ckester_results;

/* Run all registered tests */
CKESTER_WEAK int _ckester_run_all_tests(void) {
    size_t test_failures = 0;
    size_t suite_count = 0;
    size_t suite_failed_count = 0;

    for (int i = 0; i < _ckester_registry.count; i++) {
        suite_count++;
        int before_failed = _ckester_results.failed;

        if (_ckester_results.verbosity.suite) {
             printf("\033[1;36m%s\033[0m\n", _ckester_registry.tests[i].name);
        }

        /* Reset current test context */
        _ckester_results.current_test = _ckester_registry.tests[i].name; 
        _ckester_results.current_assertions = 0;

        /* Execute test suite */
        _ckester_registry.tests[i].func();

        if (_ckester_results.failed > before_failed) {
            test_failures++; 
            suite_failed_count++;
        }
    }

    if (_ckester_results.verbosity.summary) {
        ckester_log_summary_start();
        ckester_log_summary_item("SUITE", suite_count, suite_failed_count);
        ckester_log_summary_item("ASSERTION", _ckester_results.passed + _ckester_results.failed, _ckester_results.failed);
    }
    
    if (_ckester_registry.tests)
            free(_ckester_registry.tests);

    return test_failures > 0 ? 1 : 0;
}

/* Assertion Macros */
#define _CKESTER_ASSERT(condition, msg)                                  \
    do {                                                                 \
        _ckester_results.current_assertions++;                           \
        if (!(condition)) {                                              \
            _ckester_results.failed++;                                   \
            ckester_log_assertion_failure(msg, __FILE__, __LINE__);      \
        } else {                                                         \
            _ckester_results.passed++;                                   \
            if (_ckester_results.verbosity.assertions) {                 \
                ckester_log_assertion_success(msg, __FILE__, __LINE__);  \
            }                                                            \
        }                                                                \
    } while (0)

#define CKESTER_TRUE(condition, ...) \
    _CKESTER_ASSERT((condition), #condition " should be true")
#define CKESTER_FALSE(condition, ...) \
    _CKESTER_ASSERT(!(condition), #condition " should be false")
#define CKESTER_EQ(a, b, ...) \
    _CKESTER_ASSERT((a) == (b), #a " should equal " #b)
#define CKESTER_NEQ(a, b, ...) \
    _CKESTER_ASSERT((a) != (b), #a " should not equal " #b)
#define CKESTER_NULL(ptr, ...) \
    _CKESTER_ASSERT((ptr) == NULL, #ptr " should be NULL")
#define CKESTER_NOT_NULL(ptr, ...) \
    _CKESTER_ASSERT((ptr) != NULL, #ptr " should not be NULL")
#define CKESTER_STR_EQ(a, b, ...) \
    _CKESTER_ASSERT(strcmp((a), (b)) == 0, #a " should equal " #b)
#define CKESTER_STR_NEQ(a, b, ...) \
    _CKESTER_ASSERT(strcmp((a), (b)) != 0, #a " should not equal " #b)

/* Auto-registration helper */
#if defined(__GNUC__) || defined(__clang__)
#define _CKESTER_CONSTRUCTOR __attribute__((constructor))
#elif defined(_MSC_VER)
#define _CKESTER_CONSTRUCTOR
#pragma section(".CRT$XCU", read)
#define _CKESTER_MSVC_INIT(f, p)                             \
    static void f(void);                                     \
    __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
    __pragma(comment(linker, "/include:" p #f "_")) static void f(void)
#else
#define _CKESTER_CONSTRUCTOR
#warning "Auto-registration not supported on this compiler."
#endif

/* Suite definition macro */
#ifdef _MSC_VER
#define CKESTER_SUITE(name, type_level)                      \
    static void ckester_suite_##name(void);                  \
    _CKESTER_MSVC_INIT(_init_##name, "") {                   \
        _ckester_register_test(ckester_suite_##name, #name); \
    }                                                        \
    static void ckester_suite_##name(void)
#else
#define CKESTER_SUITE(name, type_level)                      \
    static void ckester_suite_##name(void);                  \
    _CKESTER_CONSTRUCTOR static void _init_##name(void) {    \
        _ckester_register_test(ckester_suite_##name, #name); \
    }                                                        \
    static void ckester_suite_##name(void)
#endif

/* Test definition macro with loop for result logging */
#define CKESTER_TEST(name) \
    for (_ckester_test_begin(#name); _ckester_test_should_run(); _ckester_test_end())

CKESTER_WEAK int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            ckester_results_enable_all_verbosity();
            break;
        }
    }
    return _ckester_run_all_tests();
}

#endif /* __CKESTER_FEATURES_LIBRARY_MACROS_C__ */
