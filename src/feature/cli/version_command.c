#ifndef __CKESTER_FEATURE_CLI_VERSION_COMMAND_C__
#define __CKESTER_FEATURE_CLI_VERSION_COMMAND_C__

#include <stdio.h>

#include "../../core/contracts.c"

int ckester_version_command(Ckester_CliContext *ctx) {
  printf("ckester version 0.1.0\n");
  return 0;
}

#endif