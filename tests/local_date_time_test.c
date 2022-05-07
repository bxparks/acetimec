#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_to_epoch_seconds)
{
  int32_t seconds = atc_to_epoch_seconds(1931, 12, 13, 20, 45, 53);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  seconds = atc_to_epoch_seconds(2000, 1, 1, 0, 0, 0);
  ACU_ASSERT(seconds == 0);

  seconds = atc_to_epoch_seconds(2000, 1, 2, 0, 0, 0);
  ACU_ASSERT(seconds == 86400);

  seconds = atc_to_epoch_seconds(2000, 2, 29, 0, 0, 0);
  ACU_ASSERT(seconds == 86400 * 59);

  seconds = atc_to_epoch_seconds(2018, 1, 1, 0, 0, 0);
  ACU_ASSERT(seconds == 86400 * 6575);

  seconds = atc_to_epoch_seconds(2038, 1, 19, 3, 14, 7);
  ACU_ASSERT(seconds == 1200798847);

  ACU_PASS();
}

ACU_TEST(test_from_epoch_seconds_2000)
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
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 1);
  ACU_ASSERT(day == 1);
  ACU_ASSERT(hour == 0);
  ACU_ASSERT(minute == 0);
  ACU_ASSERT(second == 0);

  ACU_PASS();
}

ACU_TEST(test_from_epoch_seconds_2029)
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
  ACU_ASSERT(year == 2029);
  ACU_ASSERT(month == 12);
  ACU_ASSERT(day == 31);
  ACU_ASSERT(hour == 23);
  ACU_ASSERT(minute == 59);
  ACU_ASSERT(second == 59);

  ACU_PASS();
}

ACU_TEST(test_from_epoch_seconds_2068)
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
  ACU_ASSERT(year == 2068);
  ACU_ASSERT(month == 1);
  ACU_ASSERT(day == 19);
  ACU_ASSERT(hour == 3);
  ACU_ASSERT(minute == 14);
  ACU_ASSERT(second == 6);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  ACU_RUN_TEST(test_to_epoch_seconds);
  ACU_RUN_TEST(test_from_epoch_seconds_2000);
  ACU_RUN_TEST(test_from_epoch_seconds_2029);
  ACU_RUN_TEST(test_from_epoch_seconds_2068);
  ACU_SUMMARY();
}
