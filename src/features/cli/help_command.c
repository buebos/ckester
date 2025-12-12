#ifndef __CKESTER_FEATURES_CLI_HELP_COMMAND_C__
#define __CKESTER_FEATURES_CLI_HELP_COMMAND_C__

#include "../../core/contracts.c"
#include <stdio.h>

int ckester_help_command(Ckester_CliContext *ctx) {
  printf("Ckester - The C Test Runner\n\n");
  printf("Usage:\n");
  printf("  ckester [command] [options]\n\n");
  printf("Commands:\n");
  printf("  run     Run tests\n");
  printf("  help    Show this help message\n");
  printf("  version Show version information\n\n");
  printf("Options:\n");
  printf(
      "  -v, --verbosity <levels>  Set verbosity levels (comma separated)\n");
  printf("                            (build, bin, src, suite, test, assert, "
         "stdout, summary)\n");
  printf("  -a, --asserts             Enable assertion verbosity\n");
  return 0;
}

#endif