/*
 * A minimal unit testing framework for the C language. Depends on <stdio.h>.
 *
 * Adapted from https://jera.com/techinfo/jtns/jtn002 which has the following
 * license: "You may use the code in this tech note for any purpose, with the
 * understanding that it comes with NO WARRANTY."
 */

#include <stdint.h>
#include <stdio.h>

/**
 * Return value of each test function. If an assertion fails, this struct
 * contains the file name, line number, and a string representation of the
 * condition expression that was asserted.
 */
struct mu_result {
  const char *file;
  uint16_t line;
  const char *condition;
  const char *message;
};

/** Define a test function that returns a 'struct mu_result'. */
#define mu_test(name) \
  struct mu_result name()

/**
 * Implement the 1-argument mu_assert() macro. If we try to use a CPP trick to
 * overload mu_assert() to take 1 or 2 arguments, we get a GCC error message
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

/** Implement the 2-argument mu_assert() macro with a message. */
#define mu_assert_msg(condition, message) \
  do { \
    if (!(condition)) { \
      struct mu_result result = {__FILE__, __LINE__, #condition, message}; \
      return result; \
    } \
  } while (0)

/** Return from the test function with a success code. */
#define mu_pass() \
  do { \
    struct mu_result result = { NULL, 0, NULL, NULL }; \
    return result; \
  } while (0)

/**
 * Run the given 'test' function. If an assertion fails, print the diagnostic
 * message that was returned.
 */
#define mu_run_test(test) \
  do { \
    struct mu_result result = test(); \
    tests_run++; \
    if (result.file) { \
      printf("%s:%d: Assertion failed: [%s] is false", \
          result.file, result.line, result.condition); \
      tests_failed++; \
      if (result.message) { \
        printf(": %s\n", result.message); \
      } else { \
        printf("\n"); \
      } \
    } \
  } while (0)

/** Print out the test summary. */
#define mu_summary() \
  do { \
    if (tests_failed) { \
      printf("FAILED: %d failed out of %d test(s)\n", \
          tests_failed, tests_run); \
    } else { \
      printf("ALL PASSED: %d tests(s)\n", tests_run); \
    } \
    return tests_failed != 0; \
  } while (0)

extern int tests_run;
extern int tests_failed;
