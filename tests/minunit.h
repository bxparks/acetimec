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

/* Implement the 1-argument mu_assert() macro. If we try to use a CPP trick
 * to overload mu_assert() to take 1 or 2 arguments, we get a GCC error message
 * saying that 'ISO C99 requires at least one argument for the "..." in a
 * variadic macro'. So we have to use a separate mu_assert_msg() for the 2
 * parameter version.
 */
#define mu_assert(condition) \
  do { \
    if (!(condition)) { \
      struct mu_result result = {__FILE__, __LINE__, #condition, NULL}; \
      return result; \
    } \
  } while (0)

/* Implement the 2-argument mu_assert() macro with a message. */
#define mu_assert_msg(condition, message) \
  do { \
    if (!(condition)) { \
      struct mu_result result = {__FILE__, __LINE__, #condition, message}; \
      return result; \
    } \
  } while (0)

/* Return from the test function with a success code. */
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
    if (result.file) { \
      printf("%s:%d: Assertion failed: [%s] is false", \
          result.file, result.line, result.condition); \
      if (result.message) { \
        printf(": %s\n", result.message); \
      } else { \
        printf("\n"); \
      } \
      printf("FAILED"); \
    } else { \
      printf("ALL PASSED"); \
    } \
    printf(": Processed %d tests\n", tests_run); \
    return result.file != 0; \
  } while (0)

extern int tests_run;
