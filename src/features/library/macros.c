#ifndef __CKESTER_FEATURES_LIBRARY_MACROS_C__
#define __CKESTER_FEATURES_LIBRARY_MACROS_C__

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include "../../core/contracts.c"

/* External references to library state */
extern Ckester_TestResults _ckester_results;
extern void _ckester_register_test(void (*test)(void), const char* name);
extern int _ckester_run_all_tests(void);
extern void ckester_results_enable_all_verbosity(void);

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