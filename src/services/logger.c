#ifndef __CKESTER_SERVICES_LOGGER_C__
#define __CKESTER_SERVICES_LOGGER_C__

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

/* ANSI Color Codes */
#define CKESTER_COLOR_RESET "\033[0m"
#define CKESTER_COLOR_RED "\033[1;31m"
#define CKESTER_COLOR_GREEN "\033[1;32m"
#define CKESTER_COLOR_YELLOW "\033[1;33m"
#define CKESTER_COLOR_BLUE "\033[1;34m"
#define CKESTER_COLOR_CYAN "\033[1;36m"
#define CKESTER_COLOR_BOLD "\033[1m"
#define CKESTER_COLOR_DIM "\033[2m"

/* Logger Symbols */
#define CKESTER_SYMBOL_CHECK "✓"
#define CKESTER_SYMBOL_CROSS "✗"

typedef enum {
    CKESTER_LOG_INFO,
    CKESTER_LOG_SUCCESS,
    CKESTER_LOG_WARNING,
    CKESTER_LOG_ERROR,
    CKESTER_LOG_DEBUG
} Ckester_LogLevel;

/* Configuration for the logger (singleton-ish state) */
typedef struct {
    bool use_colors;
    bool debug_mode;
} Ckester_LoggerConfig;

static Ckester_LoggerConfig _ckester_logger_config = {true, false};

static void ckester_logger_configure(bool use_colors, bool debug_mode) {
    _ckester_logger_config.use_colors = use_colors;
    _ckester_logger_config.debug_mode = debug_mode;
}

static void ckester_log_raw(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

static void ckester_log_header(const char* tag, const char* fmt, ...) {
    if (_ckester_logger_config.use_colors) {
        printf("%s[%s]:%s ", CKESTER_COLOR_CYAN, tag, CKESTER_COLOR_RESET);
    } else {
        printf("[%s]: ", tag);
    }

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

static void ckester_log_suite_start(const char* suite_name, bool failed) {
    if (_ckester_logger_config.use_colors) {
        if (failed) {
            printf("%s%s %s%s\n", CKESTER_COLOR_RED, CKESTER_SYMBOL_CROSS, suite_name,
                   CKESTER_COLOR_RESET);
        } else {
            printf("%s%s %s%s\n", CKESTER_COLOR_GREEN, CKESTER_SYMBOL_CHECK, suite_name,
                   CKESTER_COLOR_RESET);
        }
    } else {
        printf("%s %s\n", failed ? "X" : "V", suite_name);
    }
}

static void ckester_log_test_result(const char* test_name, bool failed) {
    if (_ckester_logger_config.use_colors) {
        if (failed) {
            printf("    %s%s %s%s\n", CKESTER_COLOR_RED, CKESTER_SYMBOL_CROSS, test_name,
                   CKESTER_COLOR_RESET);
        } else {
            printf("    %s%s %s%s\n", CKESTER_COLOR_GREEN, CKESTER_SYMBOL_CHECK,
                   test_name, CKESTER_COLOR_RESET);
        }
    } else {
        printf("    %s %s\n", failed ? "X" : "V", test_name);
    }
}

static void ckester_log_assertion_failure(const char* msg, const char* file, int line) {
    if (_ckester_logger_config.use_colors) {
        printf("        %s%s %s%s\n", CKESTER_COLOR_RED, CKESTER_SYMBOL_CROSS, msg,
               CKESTER_COLOR_RESET);
        printf("        %sat %s:%d%s\n", CKESTER_COLOR_DIM, file, line,
               CKESTER_COLOR_RESET);
    } else {
        printf("        X %s\n", msg);
        printf("        at %s:%d\n", file, line);
    }
}

static void ckester_log_assertion_success(const char* msg, const char* file, int line) {
    if (_ckester_logger_config.use_colors) {
        printf("        %s%s %s%s\n", CKESTER_COLOR_GREEN, CKESTER_SYMBOL_CHECK, msg,
               CKESTER_COLOR_RESET);
        printf("        %sat %s:%d%s\n", CKESTER_COLOR_DIM, file, line,
               CKESTER_COLOR_RESET);
    } else {
        printf("        V %s\n", msg);
        printf("        at %s:%d\n", file, line);
    }
}

static void ckester_log_summary_start() {
    if (_ckester_logger_config.use_colors) {
        printf("\n%s[SUMMARY]:%s\n", CKESTER_COLOR_BOLD, CKESTER_COLOR_RESET);
    } else {
        printf("\n[SUMMARY]:\n");
    }
}

static void ckester_log_summary_item(const char* label, size_t total, size_t failed) {
    // Format:     [SUITE]: 2, 1 failed
    // Or:         [SUITE]: 2
    printf("    ");
    if (_ckester_logger_config.use_colors) {
        printf("%s[%s]:%s %zu", CKESTER_COLOR_CYAN, label, CKESTER_COLOR_RESET, total);
    } else {
        printf("[%s]: %zu", label, total);
    }

    if (failed > 0) {
        if (_ckester_logger_config.use_colors) {
            printf(", %s%zu failed%s", CKESTER_COLOR_RED, failed, CKESTER_COLOR_RESET);
        } else {
            printf(", %zu failed", failed);
        }
    }
    printf("\n");
}

static void ckester_log_summary_generic(const char* label, size_t count) {
     printf("    ");
    if (_ckester_logger_config.use_colors) {
        printf("%s[%s]:%s %zu", CKESTER_COLOR_CYAN, label, CKESTER_COLOR_RESET, count);
    } else {
        printf("[%s]: %zu", label, count);
    }
    printf("\n");
}

#endif /* __CKESTER_SERVICES_LOGGER_C__ */
