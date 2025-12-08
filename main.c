#include <stdio.h>
#include <string.h>

#include "src/core/contracts.c"
#include "src/feature/cli/help_command.c"
#include "src/feature/cli/run_command.c"
#include "src/feature/cli/version_command.c"

int main(int argc, char *argv[]) {
  Ckester_CliContext ctx = {
      .verbosity =
          {
              .build = true,
              .bin = true,
              .src = true,
              .suite = true,
              .test = true,
              .assertions = false,
              .stdout = true,
              .summary = true,
          },
      .test_dir = "test/suite",
      .build_dir = "build/ckester",
      .paths = "test/suite",
  };

  if (argc <= 1 || strcmp(argv[1], "help") == 0) {
    return ckester_help_command(&ctx);
  }

  if (strcmp(argv[1], "version") == 0) {
    return ckester_version_command(&ctx);
  }
  if (strcmp(argv[1], "run") == 0) {
    return ckester_run_command(&ctx);
  }

  fprintf(stderr, "[ERROR]: Invalid command: '%s'\n", argv[1]);

  return 1;
}
