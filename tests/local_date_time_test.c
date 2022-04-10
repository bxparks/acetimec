#include "minunit.h"
#include <acetimec.h>

mu_test(test_to_epoch_seconds)
{
  int32_t seconds = atc_to_epoch_seconds(1931, 12, 13, 20, 45, 53);
  mu_assert(seconds == INT32_MIN + 1);

  seconds = atc_to_epoch_seconds(2000, 1, 1, 0, 0, 0);
  mu_assert(seconds == 0);

  seconds = atc_to_epoch_seconds(2000, 1, 2, 0, 0, 0);
  mu_assert(seconds == 86400);

  seconds = atc_to_epoch_seconds(2000, 2, 29, 0, 0, 0);
  mu_assert(seconds == 86400 * 59);

  seconds = atc_to_epoch_seconds(2018, 1, 1, 0, 0, 0);
  mu_assert(seconds == 86400 * 6575);

  seconds = atc_to_epoch_seconds(2038, 1, 19, 3, 14, 7);
  mu_assert(seconds == 1200798847);

  mu_pass();
}

mu_test(test_from_epoch_seconds_2000)
{
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  atc_from_epoch_seconds(
    0,
    &year,
    &month,
    &day,
    &hour,
    &minute,
    &second);
  mu_assert(year == 2000);
  mu_assert(month == 1);
  mu_assert(day == 1);
  mu_assert(hour == 0);
  mu_assert(minute == 0);
  mu_assert(second == 0);

  mu_pass();
}

mu_test(test_from_epoch_seconds_2029)
{
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  atc_from_epoch_seconds(
    10958 * 86400 - 1,
    &year,
    &month,
    &day,
    &hour,
    &minute,
    &second);
  mu_assert(year == 2029);
  mu_assert(month == 12);
  mu_assert(day == 31);
  mu_assert(hour == 23);
  mu_assert(minute == 59);
  mu_assert(second == 59);

  mu_pass();
}

mu_test(test_from_epoch_seconds_2068)
{
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  atc_from_epoch_seconds(
    INT32_MAX - 1,
    &year,
    &month,
    &day,
    &hour,
    &minute,
    &second);
  mu_assert(year == 2068);
  mu_assert(month == 1);
  mu_assert(day == 19);
  mu_assert(hour == 3);
  mu_assert(minute == 14);
  mu_assert(second == 6);

  mu_pass();
}

//---------------------------------------------------------------------------

int tests_run = 0;

mu_test(all_tests)
{
  mu_run_test(test_to_epoch_seconds);
  mu_run_test(test_from_epoch_seconds_2000);
  mu_run_test(test_from_epoch_seconds_2029);
  mu_run_test(test_from_epoch_seconds_2068);
  mu_pass();
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  mu_run_suite(all_tests);
}
