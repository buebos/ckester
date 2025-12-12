#ifndef __CKESTER_FEATURES_LIBRARY_RUNNER_C__
#define __CKESTER_FEATURES_LIBRARY_RUNNER_C__

#include "../../core/contracts.c"
#include <stdio.h>
#include <stdlib.h>

extern Ckester_TestRegistry _ckester_registry;
extern Ckester_TestResults _ckester_results;

/* Run all registered tests */
int _ckester_run_all_tests(void) {
  int test_failures = 0;

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
    printf("         %d tests, %d assertions\n", _ckester_registry.count,
           _ckester_results.passed);
    /* Simple cleanup for now, though OS handles it usually */
    if (_ckester_registry.tests)
      free(_ckester_registry.tests);
    return 0;
  } else {
    printf("\033[1;31m[FAIL]:\033[0m %d/%d tests failed\n", test_failures,
           _ckester_registry.count);
    printf("         %d assertions passed, %d failed\n",
           _ckester_results.passed, _ckester_results.failed);
    if (_ckester_registry.tests)
      free(_ckester_registry.tests);
    return 1;
  }
}

#endif
