#ifndef __CKESTER_FEATURES_LIBRARY_MACROS_C__
#define __CKESTER_FEATURES_LIBRARY_MACROS_C__

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/contracts.c"

/* Global jump buffer definition */
jmp_buf _ckester_jmp_env;

/* Global test results with default verbosity */
/* Global test results with default verbosity */
Ckester_TestResults _ckester_results = {
    0,
    0,
    0,
    NULL,
    0,
    /* Default verbosity: build, bin, src, suite, test, show_stdout, summary
       (assertions=false) */
    {true, true, true, true, true, false, true, true}};

/* Helper to enable full verbosity (debug mode) */
void ckester_results_enable_all_verbosity(void) {
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
Ckester_TestRegistry _ckester_registry = {NULL, 0, 0};

/* Register a test function with its name */
void _ckester_register_test(void (*test)(void), const char* name) {
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
void _ckester_free_registry(void) {
    if (_ckester_registry.tests != NULL) {
        free(_ckester_registry.tests);
        _ckester_registry.tests = NULL;
    }
    _ckester_registry.count = 0;
    _ckester_registry.capacity = 0;
}

extern Ckester_TestRegistry _ckester_registry;
extern Ckester_TestResults _ckester_results;

/* Run all registered tests */
int _ckester_run_all_tests(void) {
    size_t test_failures = 0;

    if (_ckester_results.verbosity.summary) {
        printf("\n");
    }

    for (int i = 0; i < _ckester_registry.count; i++) {
        int before_failed = _ckester_results.failed;

        if (_ckester_results.verbosity.test) {
            /* If explicit test verbosity is on, maybe we print start?
               Prototype printed ▶ TestName.
               SRS says 'test: Show test names'.
               Let's keep the prototype behavior for 'test' flag. */
            printf("\033[1;36m▶\033[0m %s\n", _ckester_registry.tests[i].name);
        }

        /* Reset current test context */
        _ckester_results.current_test = _ckester_registry.tests[i].name;
        _ckester_results.current_assertions = 0;

        /* Execute test */
        _ckester_registry.tests[i].func();

        if (_ckester_results.failed > before_failed) {
            test_failures++;
            /* Always print failures, or maybe if verbosity.test/assertions?
               SRS says "Failed assertions shall report...".
               Usually failures are loud.
               Prototype logic: if (!verbose) print FAIL line.
               If verbose, it printed assertions.
               Now we have independent flags.
               If `verbose.test` is ON, we printed "▶ Name".
               If invalid/failed, do we print "✗ Name"?
               Let's print "✗ Name" if NOT verbosity.test (because "▶ Name" is already
               there?) Actually, if we printed "▶ Name", we might want "✗ Name" too?
               For now, preserving prototype logic:
               If NOT verbose, print result line.
               If verbose, we printed start line and assertions.
               Let's stick to: If `verbosity.test` is FALSE, then we print the result
               line. Wait, if `verbosity.test` is TRUE, we print "▶ Name". So we
               should probably NOT print "✗ Name" again? Or maybe "✗ Name" is the
               result. Let's simplify: If we showed start "▶", we don't need "✗"
               unless we want to show end status. Let's use (!verbosity.test) for now.
             */
            if (!_ckester_results.verbosity.test) {
                fprintf(stderr, "\033[1;31m✗\033[0m %s\n",
                        _ckester_registry.tests[i].name);
            }
        } else {
            if (!_ckester_results.verbosity.test) {
                printf("\033[1;32m✓\033[0m %s\n", _ckester_registry.tests[i].name);
            }
        }

        if (_ckester_results.verbosity.summary) {
            printf("\n");
        }
    }

    printf("\n");

    if (test_failures == 0) {
        printf("[INFO]:  All tests passed\n");
        printf("         %zu tests, %zu assertions\n", _ckester_registry.count,
               _ckester_results.passed);
        /* Simple cleanup for now, though OS handles it usually */
        if (_ckester_registry.tests)
            free(_ckester_registry.tests);
        return 0;
    } else {
        printf("\033[1;31m[FAIL]:\033[0m %zu/%zu tests failed\n", test_failures,
               _ckester_registry.count);
        printf("         %zu assertions passed, %zu failed\n",
               _ckester_results.passed, _ckester_results.failed);
        if (_ckester_registry.tests)
            free(_ckester_registry.tests);
        return 1;
    }
}

/* Assertion Macros */
#define _CKESTER_ASSERT(condition, msg)                                  \
    do {                                                                 \
        _ckester_results.current_assertions++;                           \
        if (!(condition)) {                                              \
            _ckester_results.failed++;                                   \
            if (_ckester_results.verbosity.assertions) {                 \
                fprintf(stderr, "    \033[1;31m✗\033[0m %s\n", msg);     \
                fprintf(stderr, "      at %s:%d\n", __FILE__, __LINE__); \
            }                                                            \
        } else {                                                         \
            _ckester_results.passed++;                                   \
            if (_ckester_results.verbosity.assertions) {                 \
                fprintf(stdout, "    \033[1;32m✓\033[0m %s\n", msg);     \
                fprintf(stdout, "      at %s:%d\n", __FILE__, __LINE__); \
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

/* Test definition macro
 * Uses setjmp to define a failure handling point.
 * IF setjmp returns 0: execute the block.
 * IF setjmp returns non-zero (via longjmp): assertion failed, block entry
 * skipped (effectively break).
 */
#define CKESTER_TEST(name)                          \
    _ckester_results.current_test = #name;          \
    if (_ckester_results.verbosity.test) {          \
        printf("  \033[1;36m▶\033[0m %s\n", #name); \
    }

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            ckester_results_enable_all_verbosity();
            break;
        }
    }
    return _ckester_run_all_tests();
}

#endif /* __CKESTER_FEATURES_LIBRARY_MACROS_C__ */