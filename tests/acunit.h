/*
 * A minimal unit testing framework for the C language.
 *
 * Adapted from "minunit.h" from https://jera.com/techinfo/jtns/jtn002 which has
 * the following license: "You may use the code in this tech note for any
 * purpose, with the understanding that it comes with NO WARRANTY."
 *
 * The ACU_SUMMARY() prints a summary that looks like this upon success:
 *
 * ```
 * PASSED: test_atc_registrar_is_registry_sorted
 * PASSED: test_atc_registrar_find_by_id_sorted
 * PASSED: test_atc_registrar_find_by_name_sorted
 * PASSED: test_atc_registrar_find_by_id_unsorted
 * PASSED: test_atc_registrar_find_by_name_unsorted
 * Summary: PASSED: 5 tests(s)
 * ```
 * and like this upon a failure:
 *
 * ```
 * PASSED: test_atc_registrar_is_registry_sorted
 * PASSED: test_atc_registrar_find_by_id_sorted
 * PASSED: test_atc_registrar_find_by_name_sorted
 * PASSED: test_atc_registrar_find_by_id_unsorted
 * zone_registrar_test.c:94: Assertion failed: [info != NULL] is false
 * FAILED: test_atc_registrar_find_by_name_unsorted
 * Summary: FAILED: 1 failed out of 5 test(s)
 * ```
 */

#include <stdio.h>

/**
 * Used to return the pass (1) or fail (0) status of a specific unit test. We
 * use a global variable to return this status for simplicity. If we returned
 * the status using a return value from the test function (i.e. making
 * ACU_TEST() define a function that returns an 'int'), then we are forced to
 * end each test function with a 'return 1' statement. We can create an
 * ACU_PASS() macro, but regardless, this return statement is very easy to
 * forget, and it quickly becomes annoying. Using a global variable allows the
 * unit test functions to have a similar structure to most other unit testing
 * frameworks from other languages.
 *
 * The disadvantage of using a global variable is that it prevents ACUnit from
 * supporting multiple threads. It might be possible to use thread-local
 * variables for this, but I have not spent any time researching this because my
 * unit test suites are small enough that I don't need multiple threads.
 */
extern int acu_test_status;

/** Number of unit tests that were executed. */
extern int acu_tests_executed;

/** Number of unit tests that failed. */
extern int acu_tests_failed;

/** Define the global variables required by ACUnit. */
#define ACU_VARS() \
  int acu_test_status = 0; \
  int acu_tests_executed = 0; \
  int acu_tests_failed = 0

/** Define a test function that returns 0 on failure, 1 on success.. */
#define ACU_TEST(name) void name(void)

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
 * Implement the 1-argument ACU_ASSERT() macro. If we try to use a CPP trick
 * (https://stackoverflow.com/questions/11761703) to overload ACU_ASSERT() to
 * take 1 or 2 arguments, we get a GCC error message saying that 'ISO C99
 * requires at least one argument for the "..." in a variadic macro'. So we have
 * to use a separate acu_assert_msg() for the 2 parameter version.
 */
#define ACU_ASSERT(condition) \
  do { \
    if (!(condition)) { \
      acu_assertion(__FILE__, __LINE__, #condition, NULL); \
      acu_test_status = 0; \
      return; \
    } \
  } while (0)

/** Implement the 2-argument ACU_ASSERT() macro with a message. */
#define ACU_ASSERT_MSG(condition, message) \
  do { \
    if (!(condition)) { \
      acu_assertion(__FILE__, __LINE__, #condition, message); \
      acu_test_status = 0; \
      return; \
    } \
  } while (0)

/**
 * Run the given 'test' function. If an assertion fails, print the diagnostic
 * message that was returned.
 */
#define ACU_RUN_TEST(test) \
  do { \
    acu_test_status = 1; \
    test(); \
    acu_tests_executed++; \
    printf("%s: %s\n", (acu_test_status ? "PASSED" : "FAILED"), #test); \
  } while (0)

/** Print out the test summary. */
#define ACU_SUMMARY() \
  do { \
    if (acu_tests_failed) { \
      printf("Summary: FAILED: %d failed out of %d test(s)\n", \
          acu_tests_failed, acu_tests_executed); \
    } else { \
      printf("Summary: PASSED: %d tests(s)\n", acu_tests_executed); \
    } \
    return acu_tests_failed != 0; \
  } while (0)
