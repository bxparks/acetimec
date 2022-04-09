/*
 * A minimal unit testing framework for the C language. Depends on <stdio.h>.
 *
 * Adapted from https://jera.com/techinfo/jtns/jtn002 which has the following
 * license: "You may use the code in this tech note for any purpose, with the
 * understanding that it comes with NO WARRANTY."
 */

#include <stdint.h>
#include <stdio.h>

struct mu_result {
  const char *file;
  uint16_t line;
  const char *condition;
  const char *message;
};

#define mu_test(name) \
  struct mu_result name()

#define mu_assert(message, test) \
  do { \
    if (!(test)) { \
      struct mu_result result = {__FILE__, __LINE__, #test, message}; \
      return result; \
    } \
  } while (0)

#define mu_pass() \
  do { \
    struct mu_result result = { NULL, 0, NULL, NULL }; \
    return result; \
  } while (0)

#define mu_run_test(test) \
  do { \
    struct mu_result result = test(); \
    tests_run++; \
    if (result.file) return result; \
  } while (0)

#define mu_run_suite(suite) \
  do { \
    struct mu_result result = suite(); \
    if (result.file != 0) { \
      printf("%s:%d: ERROR: [%s] is false: %s\n", \
          result.file, result.line, result.condition, result.message); \
    } else { \
      printf("ALL TESTS PASSED\n"); \
    } \
    printf("Tests run: %d\n", tests_run); \
    return result.file != 0; \
  } while (0)

extern int tests_run;
