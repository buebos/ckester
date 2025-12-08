#ifndef __CKESTER_MODELS_INDEX_C__
#define __CKESTER_MODELS_INDEX_C__

#include <stdbool.h>

typedef enum Ckester_TestType {
  CKESTER_TEST_TYPE_FUNCTIONAL,
  CKESTER_TEST_TYPE_PERFORMANCE,
  CKESTER_TEST_TYPE_SECURITY,
} Ckester_TestType;

typedef enum Ckester_TestLevel {
  CKESTER_TEST_LEVEL_UNIT,
  CKESTER_TEST_LEVEL_INTEGRATION,
  CKESTER_TEST_LEVEL_SYSTEM,
} Ckester_TestLevel;

typedef struct Ckester_TestSuite {
  char *id;
  char *name;
  Ckester_TestType type;
  Ckester_TestLevel level;
} Ckester_TestSuite;

typedef struct Ckester_TestCase {
  char *id;
  char *name;
} Ckester_TestCase;

typedef struct Ckester_TestResult {
  char *id;
  char *name;
} Ckester_TestResult;

#endif