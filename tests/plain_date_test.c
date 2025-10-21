#include <acunit.h>
#include <acetimec.h>

// Make sure that the default current epoch year is correctly configured to 2050
// for the purposes of all the tests below. Don't call
// `atc_set_current_epoch_year(2050)` in this test, and make sure it is executed
// before any other test that calls `atc_set_current_epoch_year()`.
ACU_TEST(test_current_epoch_year) {
  ACU_ASSERT(atc_plain_date_to_epoch_days(2050, 1, 1) == 0);
}

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

// Check (year, month, day) validation code.
ACU_TEST(test_plain_date_is_valid)
{
  // valid dates
  ACU_ASSERT(atc_plain_date_is_valid(2000, 1, 1));
  ACU_ASSERT(atc_plain_date_is_valid(2000, 2, 1));
  ACU_ASSERT(atc_plain_date_is_valid(2000, 2, 28));
  ACU_ASSERT(atc_plain_date_is_valid(2000, 2, 29));
  ACU_ASSERT(atc_plain_date_is_valid(2000, 12, 31));
  ACU_ASSERT(atc_plain_date_is_valid(2004, 2, 29));

  // invalid dates
  ACU_ASSERT(! atc_plain_date_is_valid(0, 1, 1));
  ACU_ASSERT(! atc_plain_date_is_valid(2000, 0, 1));
  ACU_ASSERT(! atc_plain_date_is_valid(2000, 1, 32));
  ACU_ASSERT(! atc_plain_date_is_valid(2000, 2, 30));
  ACU_ASSERT(! atc_plain_date_is_valid(2000, 12, 32));
  ACU_ASSERT(! atc_plain_date_is_valid(2001, 2, 29));
  ACU_ASSERT(! atc_plain_date_is_valid(2100, 2, 29));
  ACU_ASSERT(! atc_plain_date_is_valid(10000, 1, 1));
}

// Test round-trip of conversions for every day from 0001-01-01 to 9999-12-31.
// To avoid code duplication and to reduce CPU time, we can test both
// the epoch days and Unix days at the same time. In other words, we can test
// the following conversions:
//
// * atc_plain_date_to_epoch_days() and atc_plain_date_from_epoch_days()
// * atc_plain_date_to_unix_days() and atc_plain_date_from_unix_days()
ACU_TEST(test_plain_date_epoch_days_unix_days)
{
  // Number of days from 0001-01-01 to default epoch year of 2050:
  //    = 146097*5 [2000 years, 5*400 cycle]
  //    + 365 * 50 [days from 2000 to 2050]
  //    + 13 [13 leap days from 2000 to 2050]
  //    - 366 [year 0 was a leap year]
  // We need the negative of that.
  int32_t epoch_days = -(146097*5 + 365*50 + 13 - 366);

  // Number of days from 0001-01-01 to Unix epoch year 1970:
  //    = 146097*5 [2000 years, 5*400 cycle]
  //    - kAtcDaysToInternalEpochFromUnixEpoch
  //    - 366 [year 0 was a leap year]
  // We need the negative of that.
  int32_t unix_days = -(146097*5 - kAtcDaysToInternalEpochFromUnixEpoch - 366);

  for (int16_t year = 1; year <= 9999; year++) {
    for (uint8_t month = 1; month <= 12; month++) {
      uint8_t days_in_month = atc_plain_date_days_in_year_month(year, month);
      for (uint8_t day = 1; day <= days_in_month; day++) {
        // atc_plain_date_to_epoch_days()
        int32_t obs_epoch_days = atc_plain_date_to_epoch_days(year, month, day);
        ACU_ASSERT(epoch_days == obs_epoch_days);

        // atc_plain_date_from_epoch_days()
        int16_t obs_year;
        uint8_t obs_month;
        uint8_t obs_day;
        atc_plain_date_from_epoch_days(
            epoch_days, &obs_year, &obs_month, &obs_day);
        ACU_ASSERT(year == obs_year);
        ACU_ASSERT(month == obs_month);
        ACU_ASSERT(day == obs_day);

        // atc_plain_date_to_unix_days()
        int32_t obs_unix_days = atc_plain_date_to_unix_days(year, month, day);
        ACU_ASSERT(unix_days == obs_unix_days);

        // atc_plain_date_from_unix_days()
        atc_plain_date_from_unix_days(
            unix_days, &obs_year, &obs_month, &obs_day);
        ACU_ASSERT(year == obs_year);
        ACU_ASSERT(month == obs_month);
        ACU_ASSERT(day == obs_day);

        epoch_days++;
        unix_days++;
      }
    }
  }
}

// Check that invalid (year, month, day) return kAtcInvalidEpochDays.
ACU_TEST(test_plain_date_to_epoch_days_invalid)
{
  ACU_ASSERT(atc_plain_date_to_epoch_days(0, 1, 1) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2000, 0, 1) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2000, 1, 32) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2000, 2, 30) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2000, 12, 32)
      == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2001, 2, 29) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(2100, 2, 29) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_epoch_days(10000, 1, 1) == kAtcInvalidEpochDays);
}

// Check that invalid (year, month, day) return kAtcInvalidEpochDays.
ACU_TEST(test_plain_date_to_unix_days_invalid)
{
  ACU_ASSERT(atc_plain_date_to_unix_days(0, 1, 1) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2000, 0, 1) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2000, 1, 32) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2000, 2, 30) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2000, 12, 32) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2001, 2, 29) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(2100, 2, 29) == kAtcInvalidEpochDays);
  ACU_ASSERT(atc_plain_date_to_unix_days(10000, 1, 1) == kAtcInvalidEpochDays);
}

ACU_TEST(test_day_of_week)
{
  // Check every day from year 2000 to 2499, inclusive.
  uint8_t expected = 6; // 2000-01-01 was a Saturday (6)
  for (int16_t y = 2000; y < 2500; y++) {
    for (uint8_t m = 1; m <= 12; m++) {
      uint8_t days_in_month = atc_plain_date_days_in_year_month(y, m);
      for (uint8_t d = 1; d <= days_in_month; d++) {
        uint8_t dow = atc_plain_date_day_of_week(y, m, d);
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
  int16_t year;
  uint8_t month;
  uint8_t day;

  year = 2000;
  month = 2;
  day = 28;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 29);

  year = 2000;
  month = 2;
  day = 29;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 3);
  ACU_ASSERT(day == 1);

  year = 2000;
  month = 3;
  day = 31;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 4);
  ACU_ASSERT(day == 1);

  year = 2000;
  month = 12;
  day = 31;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2001);
  ACU_ASSERT(month == 1);
  ACU_ASSERT(day == 1);

  year = 2001;
  month = 2;
  day = 28;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2001);
  ACU_ASSERT(month == 3);
  ACU_ASSERT(day == 1);

  year = 2004;
  month = 2;
  day = 28;
  atc_plain_date_increment_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2004);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 29);
}

ACU_TEST(test_decrement_one_day)
{
  int16_t year;
  uint8_t month;
  uint8_t day;

  year = 2004;
  month = 2;
  day = 29;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2004);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 28);

  year = 2001;
  month = 3;
  day = 1;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2001);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 28);

  year = 2001;
  month = 1;
  day = 1;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 12);
  ACU_ASSERT(day == 31);

  year = 2000;
  month = 4;
  day = 1;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 3);
  ACU_ASSERT(day == 31);

  year = 2000;
  month = 3;
  day = 1;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 29);

  year = 2000;
  month = 2;
  day = 29;
  atc_plain_date_decrement_one_day(&year, &month, &day);
  ACU_ASSERT(year == 2000);
  ACU_ASSERT(month == 2);
  ACU_ASSERT(day == 28);
}

ACU_TEST(test_epoch_year_and_valid_years)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();

  atc_set_current_epoch_year(2200);
  ACU_ASSERT(2150 == atc_epoch_valid_year_lower());
  ACU_ASSERT(2250 == atc_epoch_valid_year_upper());

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_current_epoch_year); // must be one of earliest
  ACU_RUN_TEST(test_iso_week_enum);
  ACU_RUN_TEST(test_is_leap_year);
  ACU_RUN_TEST(test_plain_date_is_valid);
  ACU_RUN_TEST(test_plain_date_epoch_days_unix_days);
  ACU_RUN_TEST(test_plain_date_to_epoch_days_invalid);
  ACU_RUN_TEST(test_plain_date_to_unix_days_invalid);
  ACU_RUN_TEST(test_day_of_week);
  ACU_RUN_TEST(test_increment_one_day);
  ACU_RUN_TEST(test_decrement_one_day);
  ACU_RUN_TEST(test_epoch_year_and_valid_years);
  ACU_SUMMARY();
}
