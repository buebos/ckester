#ifndef __CKESTER_FEATURES_LIBRARY_REGISTRY_C__
#define __CKESTER_FEATURES_LIBRARY_REGISTRY_C__

#include "../../core/contracts.c"
#include <stdlib.h>

/* Global registry instance */
Ckester_TestRegistry _ckester_registry = {NULL, 0, 0};

/* Register a test function with its name */
void _ckester_register_test(void (*test)(void), const char *name) {
  if (_ckester_registry.count >= _ckester_registry.capacity) {
    _ckester_registry.capacity =
        _ckester_registry.capacity == 0 ? 8 : _ckester_registry.capacity * 2;
    _ckester_registry.tests =
        realloc(_ckester_registry.tests,
                _ckester_registry.capacity * sizeof(Ckester_TestEntry));
  }
  _ckester_registry.tests[_ckester_registry.count].func = test;
  _ckester_registry.tests[_ckester_registry.count].name = name;
  _ckester_registry.count++;
}

/* Cleanup registry memory */
void _ckester_free_registry(void) {
  if (_ckester_registry.tests != NULL) {
    free(_ckester_registry.tests);
    _ckester_registry.tests = NULL;
  }
  _ckester_registry.count = 0;
  _ckester_registry.capacity = 0;
}

#endif
