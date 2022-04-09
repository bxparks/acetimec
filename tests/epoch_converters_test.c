#include <stdio.h>
#include "minunit.h"
#include <acetimec.h>

//---------------------------------------------------------------------------
// date_utils.h
//---------------------------------------------------------------------------

static char *test_is_leap_year()
{
  mu_assert("Year 2000 should be leap year", atc_is_leap_year(2000));
  mu_assert("Year 2004 should be leap year", atc_is_leap_year(2004));
  mu_assert("Year 2100 should not be leap year", !atc_is_leap_year(2100));
  return 0;
}

//---------------------------------------------------------------------------
// epoch_converter_julian.h
//---------------------------------------------------------------------------

// Do a round-trip atc_to_epoch_days()/atc_from_epoch_days() conversion for
// every day from 1873-01-01 to 2127-12-31, inclusive.
#if 0
static char *test_epoch_julian_converter_all_days()
{
  int32_t epoch_days = -46385; // 1873-01-01
  for (int16_t year = 2000 - 127; year <= 2000 + 127; year++) {
    int8_t year_tiny = year - 2000;
    for (uint8_t month = 1; month <= 12; month++) {
      uint8_t days_in_month = LocalDate::days_in_month(year, month);
      for (uint8_t day = 1; day <= days_in_month; day++) {
        // Test atc_to_epoch_days()
        int32_t obs_epoch_days = atc_to_epoch_days_julian(
            year_tiny, month, day);
        mu_assert(epoch_days, obs_epoch_days);

        // Test fromEopchDays()
        int8_t obs_year_tiny;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_from_epoch_days_julian(
            epoch_days, &obs_year_tiny, &obs_month, &obs_day);
        mu_assert(year_tiny, obs_year_tiny);
        mu_assert(month, obs_month);
        mu_assert(day, obs_day);

        // next epoch day
        epoch_days++;
      }
    }
  }
}
#endif

//---------------------------------------------------------------------------
// epoch_converter_hinnant2.h
//---------------------------------------------------------------------------

// Do a round-trip atc_to_epoch_days()/atc_from_epoch_days() conversion for
// every day from 1873-01-01 to 2127-12-31, inclusive.
static char *test_epoch_converter_hinnant2_all_days()
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
        mu_assert("epoch_days not equal", epoch_days == obs_epoch_days);

        // Test atc_from_epoch_days()
        int8_t obs_year_tiny;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_from_epoch_days(epoch_days, &obs_year_tiny, &obs_month, &obs_day);
        mu_assert("year_tiny not equal", year_tiny == obs_year_tiny);
        mu_assert("month not equal", month == obs_month);
        mu_assert("day not equal", day == obs_day);

        // next epoch day
        epoch_days++;
      }
    }
  }
  return 0;
}

//---------------------------------------------------------------------------

int tests_run = 0;

static char * all_tests() {
		mu_run_test(test_is_leap_year);
		mu_run_test(test_epoch_converter_hinnant2_all_days);
		return 0;
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  char *result = all_tests();
  if (result != 0) {
    printf("ERROR: %s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
