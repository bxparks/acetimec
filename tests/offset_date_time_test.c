#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_offset_date_time_to_epoch_seconds)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  // smallest valid offset_date_time
  AtcOffsetDateTime odt = {1931, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(INT32_MIN + 1 == atc_offset_date_time_to_epoch_seconds(&odt));

  // offset_date_time at exactly epoch_seconds==0
  odt = (AtcOffsetDateTime) {2000, 1, 1, 0, 0, 0, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(0 == atc_offset_date_time_to_epoch_seconds(&odt));

  // one day later
  odt = (AtcOffsetDateTime) {2000, 1, 2, 0, 0, 0, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(86400 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest value using 32-bit Unix seconds
  odt = (AtcOffsetDateTime) {2038, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(1200798847 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest valid offset_date_time using 32-bit epoch seconds from 2000
  odt = (AtcOffsetDateTime) {
    2068, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MAX == atc_offset_date_time_to_epoch_seconds(&odt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_with_offset)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  int32_t offset = 121; // 2m1s
  AtcOffsetDateTime odt = {2000, 1, 1, 0, 0, 0, 0 /*fold*/, offset};
  ACU_ASSERT(-offset == atc_offset_date_time_to_epoch_seconds(&odt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_invalid)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcOffsetDateTime odt;
  atc_offset_date_time_set_error(&odt);
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);
  ACU_ASSERT(kAtcInvalidEpochSeconds == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_to_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  // smallest valid offset_date_time
  AtcOffsetDateTime odt = {1981, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(INT32_MIN + 1 == atc_offset_date_time_to_epoch_seconds(&odt));

  // offset_date_time at exactly epoch_seconds==0
  odt = (AtcOffsetDateTime) {2050, 1, 1, 0, 0, 0, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(0 == atc_offset_date_time_to_epoch_seconds(&odt));

  // one day later
  odt = (AtcOffsetDateTime) {2050, 1, 2, 0, 0, 0, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(86400 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest valid offset_date_time given 32-bit epoch_seconds
  odt = (AtcOffsetDateTime) {2118, 1, 20, 3, 14, 7, 0 /*fold*/, 0 /*offset*/};
  ACU_ASSERT(INT32_MAX == atc_offset_date_time_to_epoch_seconds(&odt));

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_from_epoch_seconds_invalid)
{
  AtcOffsetDateTime odt;
  int32_t total_offset_seconds = 0;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  atc_offset_date_time_from_epoch_seconds(
      &odt, epoch_seconds, total_offset_seconds);
  ACU_ASSERT(atc_offset_date_time_is_error(&odt));
}

ACU_TEST(test_offset_date_time_from_epoch_seconds_epoch2000)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcOffsetDateTime odt;
  int32_t offset_seconds;
  atc_time_t epoch_seconds;

  offset_seconds = 0;
  epoch_seconds = 0; // 00:00:00
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2000);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  offset_seconds = 0;
  epoch_seconds = 3661; // 01:01:01 later
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2000);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 1);
  ACU_ASSERT(odt.minute == 1);
  ACU_ASSERT(odt.second == 1);

  offset_seconds = -8*3600; // UTC-08:00
  epoch_seconds = 0; // 00:00:00
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 1999);
  ACU_ASSERT(odt.month == 12);
  ACU_ASSERT(odt.day == 31);
  ACU_ASSERT(odt.hour == 16);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcOffsetDateTime odt;
  int32_t offset_seconds;
  atc_time_t epoch_seconds;

  offset_seconds = 0;
  epoch_seconds = 0; // 00:00:00
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2050);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  offset_seconds = 0;
  epoch_seconds = 3661; // 01:01:01 later
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2050);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 1);
  ACU_ASSERT(odt.minute == 1);
  ACU_ASSERT(odt.second == 1);

  offset_seconds = -8*3600; // UTC-08:00
  epoch_seconds = 0; // 00:00:00
  atc_offset_date_time_from_epoch_seconds(&odt, epoch_seconds, offset_seconds);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2049);
  ACU_ASSERT(odt.month == 12);
  ACU_ASSERT(odt.day == 31);
  ACU_ASSERT(odt.hour == 16);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_to_unix_seconds)
{
  AtcOffsetDateTime odt;
  atc_offset_date_time_set_error(&odt);
  int64_t unix_seconds = atc_offset_date_time_to_unix_seconds(&odt);
  ACU_ASSERT(unix_seconds == kAtcInvalidUnixSeconds);

  // $ date +%s -d '1900-01-01T00:00:00+00:01'
  odt = (AtcOffsetDateTime){1900, 1, 1, 0, 0, 0, 0 /*fold*/, 60 /*offset*/};
  unix_seconds = atc_offset_date_time_to_unix_seconds(&odt);
  ACU_ASSERT(unix_seconds == -2208988800L - 60);

  // $ date +%s -d '2000-01-02T03:04:05+00:01'
  odt = (AtcOffsetDateTime){2000, 1, 2, 3, 4, 5, 0 /*fold*/, 60 /*offset*/};
  unix_seconds = atc_offset_date_time_to_unix_seconds(&odt);
  ACU_ASSERT(unix_seconds == 946782245L - 60);

  // $ date +%s -d '2100-01-02T03:04:05+00:01'
  odt = (AtcOffsetDateTime){2100, 1, 2, 3, 4, 5, 0 /*fold*/, 60 /*offset*/};
  unix_seconds = atc_offset_date_time_to_unix_seconds(&odt);
  ACU_ASSERT(unix_seconds == 4102542245L - 60);
}

ACU_TEST(test_offset_date_time_from_unix_seconds)
{
  AtcOffsetDateTime odt;

  atc_offset_date_time_from_unix_seconds(&odt, kAtcInvalidUnixSeconds, 0);
  ACU_ASSERT(atc_offset_date_time_is_error(&odt));

  atc_offset_date_time_from_unix_seconds(&odt, -2208988800L - 60, 60);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 1900);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);
  ACU_ASSERT(odt.offset_seconds == 60);

  atc_offset_date_time_from_unix_seconds(&odt, 946782245L - 60, 60);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2000);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 2);
  ACU_ASSERT(odt.hour == 3);
  ACU_ASSERT(odt.minute == 4);
  ACU_ASSERT(odt.second == 5);
  ACU_ASSERT(odt.offset_seconds == 60);

  atc_offset_date_time_from_unix_seconds(&odt, 4102542245L - 60, 60);
  ACU_ASSERT(!atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2100);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 2);
  ACU_ASSERT(odt.hour == 3);
  ACU_ASSERT(odt.minute == 4);
  ACU_ASSERT(odt.second == 5);
  ACU_ASSERT(odt.offset_seconds == 60);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_with_offset);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_invalid);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds_epoch2000);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_offset_date_time_to_unix_seconds);
  ACU_RUN_TEST(test_offset_date_time_from_unix_seconds);
  ACU_SUMMARY();
}
