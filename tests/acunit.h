/*
 * A minimal unit testing framework for the C language.
 *
 * Adapted from "minunit.h" from https://jera.com/techinfo/jtns/jtn002 which has
 * the following license: "You may use the code in this tech note for any
 * purpose, with the understanding that it comes with NO WARRANTY."
 */

#include <stdint.h>
#include <stdio.h>

/**
 * Return value of each test function. If an assertion fails, this struct
 * contains the file name, line number, and a string representation of the
 * condition expression that was asserted.
 */
struct acu_result {
  const char *file;
  uint32_t line;
  const char *condition;
  const char *message;
};

/** Define a test function that returns a 'struct acu_result'. */
#define acu_test(name) \
  struct acu_result name()

/**
 * Implement the 1-argument acu_assert() macro. If we try to use a CPP trick to
 * overload acu_assert() to take 1 or 2 arguments, we get a GCC error message
 * saying that 'ISO C99 requires at least one argument for the "..." in a
 * variadic macro'. So we have to use a separate acu_assert_msg() for the 2
 * parameter version.
 */
#define acu_assert(condition) \
  do { \
    if (!(condition)) { \
      struct acu_result result = {__FILE__, __LINE__, #condition, NULL}; \
      return result; \
    } \
  } while (0)

/** Implement the 2-argument acu_assert() macro with a message. */
#define acu_assert_msg(condition, message) \
  do { \
    if (!(condition)) { \
      struct acu_result result = {__FILE__, __LINE__, #condition, message}; \
      return result; \
    } \
  } while (0)

/** Return from the test function with a success code. */
#define acu_pass() \
  do { \
    struct acu_result result = { NULL, 0, NULL, NULL }; \
    return result; \
  } while (0)

/**
 * Run the given 'test' function. If an assertion fails, print the diagnostic
 * message that was returned.
 */
#define acu_run_test(test) \
  do { \
    struct acu_result result = test(); \
    acu_tests_run++; \
    if (result.file) { \
      printf("%s:%lu: Assertion failed: [%s] is false", \
          result.file, (unsigned long) result.line, result.condition); \
      acu_tests_failed++; \
      if (result.message) { \
        printf(": %s\n", result.message); \
      } else { \
        printf("\n"); \
      } \
      printf("FAILED: %s\n", #test); \
    } else { \
      printf("PASSED: %s\n", #test); \
    } \
  } while (0)

/** Print out the test summary. */
#define acu_summary() \
  do { \
    if (acu_tests_failed) { \
      printf("Summary: FAILED: %d failed out of %d test(s)\n", \
          acu_tests_failed, acu_tests_run); \
    } else { \
      printf("Summary: PASSED: %d tests(s)\n", acu_tests_run); \
    } \
    return acu_tests_failed != 0; \
  } while (0)

/** Number of unit tests run. */
extern int acu_tests_run;

/** Number of failed unit tests. */
extern int acu_tests_failed;
