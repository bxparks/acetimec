#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_is_leap_year)
{
  ACU_ASSERT(atc_is_leap_year(2000));
  ACU_ASSERT(atc_is_leap_year(2004));
  ACU_ASSERT(!atc_is_leap_year(2100));
  ACU_PASS();
}

// Do a round-trip atc_to_epoch_days()/atc_from_epoch_days() conversion for
// every day from 1873-01-01 to 2127-12-31, inclusive.
ACU_TEST(test_to_and_from_epoch_days)
{
  int32_t epoch_days = -46385; // 1873-01-01
  for (int16_t year = 2000 - 127; year <= 2000 + 127; year++) {
    int8_t year_tiny = year - 2000;
    for (uint8_t month = 1; month <= 12; month++) {
      uint8_t days_in_month = atc_days_in_year_month(year, month);
      for (uint8_t day = 1; day <= days_in_month; day++) {
        // Test atc_to_epoch_days()
        int32_t obs_epoch_days = atc_to_epoch_days(
            year_tiny, month, day);
        ACU_ASSERT(epoch_days == obs_epoch_days);

        // Test atc_from_epoch_days()
        int8_t obs_year_tiny;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_from_epoch_days(epoch_days, &obs_year_tiny, &obs_month, &obs_day);
        ACU_ASSERT(year_tiny == obs_year_tiny);
        ACU_ASSERT(month == obs_month);
        ACU_ASSERT(day == obs_day);

        // next epoch day
        epoch_days++;
      }
    }
  }
  ACU_PASS();
}

//---------------------------------------------------------------------------

int acu_tests_run = 0;
int acu_tests_failed = 0;

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  ACU_RUN_TEST(test_is_leap_year);
  ACU_RUN_TEST(test_to_and_from_epoch_days);
  ACU_SUMMARY();
}
