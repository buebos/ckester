#ifndef __CKESTER_CORE_CONTRACTS_C__
#define __CKESTER_CORE_CONTRACTS_C__

#include <stdbool.h>

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

typedef struct Ckester_CliContext {
  struct Ckester_CliContext_OutputVerbosity {
    /*
     * Whether to print when there was a new build do to test src or application
     * src changes
     */
    bool build;
    /**
     * Whether to print the binaries used for the following test suites
     */
    bool bin;
    /**
     * Whether to print the source files used for the following test suites
     */
    bool src;
    /**
     * Whether to print the test suites names in each run
     */
    bool suite;
    /**
     * Whether to print the test cases names in each run
     */
    bool test;

    bool assertions;

    /**
     * Whether to print the stdout of either the test scripts or their code
     * being tested
     */
    bool stdout;
    /**
     * Whether to print the summary of the test run with: sums for
     * - Total builds performed
     * - Total binaries executed
     * - Total source files
     * - Total suites run, passed, failed
     * - Total tests executed, passed, failed
     * - Total assertions checked, passed, failed
     */
    bool summary;
  } verbosity;

  /**
   * A comma separated string of either file paths or directory paths to run
   * tests for. When empty, all C files and bash scripts on '$PWD/test/suite'
   * are run.
   */
  char *paths;

  char *test_dir;
  char *build_dir;
} Ckester_CliContext;

#endif