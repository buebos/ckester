#ifndef __CKESTER_FEATURES_LIBRARY_RESULTS_C__
#define __CKESTER_FEATURES_LIBRARY_RESULTS_C__

#include "../../core/contracts.c"
#include <stdio.h>
#include <stdlib.h>

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
  _ckester_results.verbosity.show_stdout = true;
  _ckester_results.verbosity.summary = true;
}

#endif
