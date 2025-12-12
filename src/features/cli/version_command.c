#ifndef __CKESTER_FEATURES_CLI_VERSION_COMMAND_C__
#define __CKESTER_FEATURES_CLI_VERSION_COMMAND_C__

#include "../../core/contracts.c"
#include <stdio.h>

int ckester_version_command(Ckester_CliContext *ctx) {
  printf("Ckester version 0.1.0\n");
  return 0;
}

#endif