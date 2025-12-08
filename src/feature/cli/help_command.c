#ifndef __CKESTER_FEATURE_CLI_HELP_COMMAND_C__
#define __CKESTER_FEATURE_CLI_HELP_COMMAND_C__

#include <stdio.h>

#include "../../core/contracts.c"

int ckester_help_command(Ckester_CliContext *ctx) {
  printf("Usage: ckester <command>\n");
  printf("Available commands:\n");
  printf("  help\n");
  printf("  run\n");
  return 0;
}

#endif