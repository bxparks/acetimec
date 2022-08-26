#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_iso_week_enum)
{
  ACU_ASSERT(1 == kAtcIsoWeekdayMonday);
  ACU_ASSERT(7 == kAtcIsoWeekdaySunday);
}

ACU_TEST(test_is_leap_year)
{
  ACU_ASSERT(!atc_is_leap_year(1999));
  ACU_ASSERT(atc_is_leap_year(2000)); // divisible by 400 => leap
  ACU_ASSERT(!atc_is_leap_year(2001));
  ACU_ASSERT(atc_is_leap_year(2004));
  ACU_ASSERT(!atc_is_leap_year(2100)); // divisible by 100 => not leap
  ACU_ASSERT(atc_is_leap_year(2400)); // divisible by 400 => leap

  ACU_ASSERT(atc_is_leap_year(0)); // divisible by 400 => leap
  ACU_ASSERT(!atc_is_leap_year(-1));
  ACU_ASSERT(atc_is_leap_year(-4));
  ACU_ASSERT(!atc_is_leap_year(-100)); // divisible by 100 => not leap
  ACU_ASSERT(atc_is_leap_year(-400)); // divisible by 100 => not leap
}

// Do a round-trip of conversion atc_local_date_to_epoch_days() and
// atc_local_date_from_epoch_days() for every day from 1873-01-01 to 2127-12-31,
// inclusive.
ACU_TEST(test_local_date_to_and_from_epoch_days)
{
  int32_t epoch_days = -46385; // 1873-01-01
  for (int16_t year = 2000 - 127; year <= 2000 + 127; year++) {
    int8_t year_tiny = year - 2000;
    for (uint8_t month = 1; month <= 12; month++) {
      uint8_t days_in_month = atc_local_date_days_in_year_month(year, month);
      for (uint8_t day = 1; day <= days_in_month; day++) {
        // Test atc_to_epoch_days()
        int32_t obs_epoch_days = atc_local_date_to_epoch_days(
            year_tiny, month, day);
        ACU_ASSERT(epoch_days == obs_epoch_days);

        // Test atc_from_epoch_days()
        int8_t obs_year_tiny;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_local_date_from_epoch_days(
            epoch_days, &obs_year_tiny, &obs_month, &obs_day);
        ACU_ASSERT(year_tiny == obs_year_tiny);
        ACU_ASSERT(month == obs_month);
        ACU_ASSERT(day == obs_day);

        // next epoch day
        epoch_days++;
      }
    }
  }
}

ACU_TEST(test_day_of_week)
{
  // Check every day from year 2000 to 2499, inclusive.
  uint8_t expected = 6; // 2000-01-01 was a Saturday (6)
  for (int16_t y = 2000; y < 2500; y++) {
    for (uint8_t m = 1; m <= 12; m++) {
      uint8_t days_in_month = atc_local_date_days_in_year_month(y, m);
      for (uint8_t d = 1; d <= days_in_month; d++) {
        uint8_t dow = atc_local_date_day_of_week(y, m, d);
        ACU_ASSERT(expected == dow);
        expected++;
        if (expected > 7) {
          expected = 1;
        }
      }
    }
  }
}

ACU_TEST(test_increment_one_day)
{
  struct AtcLocalDate ld;

  ld.year = 2000;
  ld.month = 2;
  ld.day = 28;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 29);

  ld.year = 2000;
  ld.month = 2;
  ld.day = 29;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 3);
  ACU_ASSERT(ld.day == 1);

  ld.year = 2000;
  ld.month = 3;
  ld.day = 31;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 4);
  ACU_ASSERT(ld.day == 1);

  ld.year = 2000;
  ld.month = 12;
  ld.day = 31;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2001);
  ACU_ASSERT(ld.month == 1);
  ACU_ASSERT(ld.day == 1);

  ld.year = 2001;
  ld.month = 2;
  ld.day = 28;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2001);
  ACU_ASSERT(ld.month == 3);
  ACU_ASSERT(ld.day == 1);

  ld.year = 2004;
  ld.month = 2;
  ld.day = 28;
  atc_local_date_increment_one_day(&ld);
  ACU_ASSERT(ld.year == 2004);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 29);
}

ACU_TEST(test_decrement_one_day)
{
  struct AtcLocalDate ld;

  ld.year = 2004;
  ld.month = 2;
  ld.day = 29;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2004);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 28);

  ld.year = 2001;
  ld.month = 3;
  ld.day = 1;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2001);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 28);

  ld.year = 2001;
  ld.month = 1;
  ld.day = 1;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 12);
  ACU_ASSERT(ld.day == 31);

  ld.year = 2000;
  ld.month = 4;
  ld.day = 1;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 3);
  ACU_ASSERT(ld.day == 31);

  ld.year = 2000;
  ld.month = 3;
  ld.day = 1;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 29);

  ld.year = 2000;
  ld.month = 2;
  ld.day = 29;
  atc_local_date_decrement_one_day(&ld);
  ACU_ASSERT(ld.year == 2000);
  ACU_ASSERT(ld.month == 2);
  ACU_ASSERT(ld.day == 28);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_iso_week_enum);
  ACU_RUN_TEST(test_is_leap_year);
  ACU_RUN_TEST(test_local_date_to_and_from_epoch_days);
  ACU_RUN_TEST(test_day_of_week);
  ACU_RUN_TEST(test_increment_one_day);
  ACU_RUN_TEST(test_decrement_one_day);
  ACU_SUMMARY();
}
