/*
 * A minimal unit testing framework for the C language.
 *
 * Adapted from "minunit.h" from https://jera.com/techinfo/jtns/jtn002 which has
 * the following license: "You may use the code in this tech note for any
 * purpose, with the understanding that it comes with NO WARRANTY."
 */

#include <stdio.h>

/** Number of unit tests run. */
extern int acu_tests_run;

/** Number of unit tests failed. */
extern int acu_tests_failed;

/** Define the global parameters required by ACUnit. */
#define ACU_PARAMS() \
  int acu_tests_run = 0; \
  int acu_tests_failed = 0

/** Define a test function that returns 0 on failure, 1 on success.. */
#define ACU_TEST(name) int name(void)

/**
 * Print assertion failure message. The format is compatible with the error
 * messages printed out by the C compiler. This format is recognized by editors
 * such as vim to quickly jump to the location of the error in the source file.
 *
 * It looks like the value of the `__LINE__` macro is guaranteed to fit inside a
 * `long` in C11 (https://stackoverflow.com/questions/5075928).
 */
extern inline void acu_assertion(
  const char *file,
  long line,
  const char *condition,
  const char *message)
{
  printf("%s:%ld: Assertion failed: [%s] is false", file, line, condition);
  acu_tests_failed++;
  if (message) {
    printf(": %s\n", message);
  } else {
    printf("\n");
  }
}

/**
 * Implement the 1-argument ACU_ASSERT() macro. If we try to use a CPP trick to
 * overload ACU_ASSERT() to take 1 or 2 arguments, we get a GCC error message
 * saying that 'ISO C99 requires at least one argument for the "..." in a
 * variadic macro'. So we have to use a separate acu_assert_msg() for the 2
 * parameter version.
 */
#define ACU_ASSERT(condition) \
  do { \
    if (!(condition)) { \
      acu_assertion(__FILE__, __LINE__, #condition, NULL); \
      return 0; \
    } \
  } while (0)

/** Implement the 2-argument ACU_ASSERT() macro with a message. */
#define ACU_ASSERT_MSG(condition, message) \
  do { \
    if (!(condition)) { \
      acu_assertion(__FILE__, __LINE__, #condition, message); \
      return 0; \
    } \
  } while (0)

/** Return from the test function with a success code. */
#define ACU_PASS() return 1

/**
 * Run the given 'test' function. If an assertion fails, print the diagnostic
 * message that was returned.
 */
#define ACU_RUN_TEST(test) \
  do { \
    int passed = test(); \
    acu_tests_run++; \
    printf("%s: %s\n", (passed ? "PASSED" : "FAILED"), #test); \
  } while (0)

/** Print out the test summary. */
#define ACU_SUMMARY() \
  do { \
    if (acu_tests_failed) { \
      printf("Summary: FAILED: %d failed out of %d test(s)\n", \
          acu_tests_failed, acu_tests_run); \
    } else { \
      printf("Summary: PASSED: %d tests(s)\n", acu_tests_run); \
    } \
    return acu_tests_failed != 0; \
  } while (0)
