#include "acunit.h"
#include <acetimec.h>

acu_test(test_is_leap_year)
{
  acu_assert(atc_is_leap_year(2000));
  acu_assert(atc_is_leap_year(2004));
  acu_assert(!atc_is_leap_year(2100));
  acu_pass();
}

// Do a round-trip atc_to_epoch_days()/atc_from_epoch_days() conversion for
// every day from 1873-01-01 to 2127-12-31, inclusive.
acu_test(test_to_and_from_epoch_days)
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
        acu_assert(epoch_days == obs_epoch_days);

        // Test atc_from_epoch_days()
        int8_t obs_year_tiny;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_from_epoch_days(epoch_days, &obs_year_tiny, &obs_month, &obs_day);
        acu_assert(year_tiny == obs_year_tiny);
        acu_assert(month == obs_month);
        acu_assert(day == obs_day);

        // next epoch day
        epoch_days++;
      }
    }
  }
  acu_pass();
}

//---------------------------------------------------------------------------

int acu_tests_run = 0;
int acu_tests_failed = 0;

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  acu_run_test(test_is_leap_year);
  acu_run_test(test_to_and_from_epoch_days);
  acu_summary();
}
