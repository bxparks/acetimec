#include "acunit.h"
#include <acetimec.h>

acu_test(test_to_epoch_seconds)
{
  int32_t seconds = atc_to_epoch_seconds(1931, 12, 13, 20, 45, 53);
  acu_assert(seconds == INT32_MIN + 1);

  seconds = atc_to_epoch_seconds(2000, 1, 1, 0, 0, 0);
  acu_assert(seconds == 0);

  seconds = atc_to_epoch_seconds(2000, 1, 2, 0, 0, 0);
  acu_assert(seconds == 86400);

  seconds = atc_to_epoch_seconds(2000, 2, 29, 0, 0, 0);
  acu_assert(seconds == 86400 * 59);

  seconds = atc_to_epoch_seconds(2018, 1, 1, 0, 0, 0);
  acu_assert(seconds == 86400 * 6575);

  seconds = atc_to_epoch_seconds(2038, 1, 19, 3, 14, 7);
  acu_assert(seconds == 1200798847);

  acu_pass();
}

acu_test(test_from_epoch_seconds_2000)
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
  acu_assert(year == 2000);
  acu_assert(month == 1);
  acu_assert(day == 1);
  acu_assert(hour == 0);
  acu_assert(minute == 0);
  acu_assert(second == 0);

  acu_pass();
}

acu_test(test_from_epoch_seconds_2029)
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
  acu_assert(year == 2029);
  acu_assert(month == 12);
  acu_assert(day == 31);
  acu_assert(hour == 23);
  acu_assert(minute == 59);
  acu_assert(second == 59);

  acu_pass();
}

acu_test(test_from_epoch_seconds_2068)
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
  acu_assert(year == 2068);
  acu_assert(month == 1);
  acu_assert(day == 19);
  acu_assert(hour == 3);
  acu_assert(minute == 14);
  acu_assert(second == 6);

  acu_pass();
}

//---------------------------------------------------------------------------

int acu_tests_run = 0;
int acu_tests_failed = 0;

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  acu_run_test(test_to_epoch_seconds);
  acu_run_test(test_from_epoch_seconds_2000);
  acu_run_test(test_from_epoch_seconds_2029);
  acu_run_test(test_from_epoch_seconds_2068);
  acu_summary();
}
