#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_plain_date_time_errors)
{
  AtcPlainDateTime pdt;
  atc_plain_date_time_set_error(&pdt);
  ACU_ASSERT(atc_plain_date_time_is_error(&pdt));
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_is_valid_false)
{
  AtcPlainDateTime pdt = {0, 1, 2, 3, 4, 5}; // 0000-01-02 03:04:05
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));

  pdt = (AtcPlainDateTime) {2000, 0, 2, 3, 4, 5}; // 2000-00-02 03:04:05
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));

  pdt = (AtcPlainDateTime) {2000, 1, 32, 3, 4, 5}; // 2000-01-32 03:04:05
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));

  pdt = (AtcPlainDateTime) {2000, 1, 2, 24, 4, 5}; // 2000-01-02 24:04:05
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 60, 5}; // 2000-01-02 3:60:05
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 4, 60}; // 2000-01-02 3:4:60
  ACU_ASSERT(! atc_plain_date_time_is_valid(&pdt));
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_to_epoch_seconds_epoch2000)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcPlainDateTime pdt = {1931, 12, 13, 20, 45, 53};
  int32_t seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  pdt = (AtcPlainDateTime) {2000, 1, 1, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 0);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 86400);

  pdt = (AtcPlainDateTime) {2000, 2, 29, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 86400 * 59);

  pdt = (AtcPlainDateTime) {2018, 1, 1, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 86400 * 6575);

  pdt = (AtcPlainDateTime) {2038, 1, 19, 3, 14, 7};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 1200798847);

  pdt = (AtcPlainDateTime) {2068, 1, 19, 3, 14, 7};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == INT32_MAX);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_plain_date_time_from_epoch_seconds_epoch2000)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcPlainDateTime pdt;

  atc_plain_date_time_from_epoch_seconds(&pdt, INT32_MIN + 1);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 1931);
  ACU_ASSERT(pdt.month == 12);
  ACU_ASSERT(pdt.day == 13);
  ACU_ASSERT(pdt.hour == 20);
  ACU_ASSERT(pdt.minute == 45);
  ACU_ASSERT(pdt.second == 53);

  atc_plain_date_time_from_epoch_seconds(&pdt, 0);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2000);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 1);
  ACU_ASSERT(pdt.hour == 0);
  ACU_ASSERT(pdt.minute == 0);
  ACU_ASSERT(pdt.second == 0);

  atc_plain_date_time_from_epoch_seconds(&pdt, 10958 * 86400 - 1);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2029);
  ACU_ASSERT(pdt.month == 12);
  ACU_ASSERT(pdt.day == 31);
  ACU_ASSERT(pdt.hour == 23);
  ACU_ASSERT(pdt.minute == 59);
  ACU_ASSERT(pdt.second == 59);

  atc_plain_date_time_from_epoch_seconds(&pdt, INT32_MAX - 1);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2068);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 19);
  ACU_ASSERT(pdt.hour == 3);
  ACU_ASSERT(pdt.minute == 14);
  ACU_ASSERT(pdt.second == 6);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_to_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcPlainDateTime pdt = {1981, 12, 13, 20, 45, 53};
  int32_t seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  pdt = (AtcPlainDateTime) {2050, 1, 1, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 0);

  pdt = (AtcPlainDateTime) {2050, 1, 2, 0, 0, 0};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == 86400);

  pdt = (AtcPlainDateTime) {2118, 1, 20, 3, 14, 7};
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == INT32_MAX);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_plain_date_time_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcPlainDateTime pdt;

  atc_plain_date_time_from_epoch_seconds(&pdt, INT32_MIN + 1);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 1981);
  ACU_ASSERT(pdt.month == 12);
  ACU_ASSERT(pdt.day == 13);
  ACU_ASSERT(pdt.hour == 20);
  ACU_ASSERT(pdt.minute == 45);
  ACU_ASSERT(pdt.second == 53);

  atc_plain_date_time_from_epoch_seconds(&pdt, 0);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2050);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 1);
  ACU_ASSERT(pdt.hour == 0);
  ACU_ASSERT(pdt.minute == 0);
  ACU_ASSERT(pdt.second == 0);

  atc_plain_date_time_from_epoch_seconds(&pdt, INT32_MAX - 1);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2118);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 20);
  ACU_ASSERT(pdt.hour == 3);
  ACU_ASSERT(pdt.minute == 14);
  ACU_ASSERT(pdt.second == 6);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_to_epoch_seconds_invalid)
{
  AtcPlainDateTime pdt = {0, 1, 2, 3, 4, 5}; // 0000-01-02 03:04:05
  atc_time_t seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);

  pdt = (AtcPlainDateTime) {2000, 0, 2, 3, 4, 5}; // 2000-00-02 03:04:05
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 32, 3, 4, 5}; // 2000-01-32 03:04:05
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 24, 4, 5}; // 2000-01-02 24:04:05
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 60, 5}; // 2000-01-02 3:60:05
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 4, 60}; // 2000-01-02 3:4:60
  seconds = atc_plain_date_time_to_epoch_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidEpochSeconds);
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_to_unix_seconds)
{
  AtcPlainDateTime pdt;
  atc_plain_date_time_set_error(&pdt);
  int64_t unix_seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(unix_seconds == kAtcInvalidUnixSeconds);

  // $ date +%s -d '1900-01-01T00:00:00Z'
  pdt = (AtcPlainDateTime){1900, 1, 1, 0, 0, 0};
  unix_seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(unix_seconds == -2208988800L);

  // $ date +%s -d '2000-01-02T03:04:05Z'
  pdt = (AtcPlainDateTime){2000, 1, 2, 3, 4, 5};
  unix_seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(unix_seconds == 946782245L);

  // $ date +%s -d '2100-01-02T03:04:05Z'
  pdt = (AtcPlainDateTime){2100, 1, 2, 3, 4, 5};
  unix_seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(unix_seconds == 4102542245L);
}

ACU_TEST(test_plain_date_time_from_unix_seconds)
{
  AtcPlainDateTime pdt;

  atc_plain_date_time_from_unix_seconds(&pdt, kAtcInvalidUnixSeconds);
  ACU_ASSERT(atc_plain_date_time_is_error(&pdt));

  atc_plain_date_time_from_unix_seconds(&pdt, -2208988800L);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 1900);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 1);
  ACU_ASSERT(pdt.hour == 0);
  ACU_ASSERT(pdt.minute == 0);
  ACU_ASSERT(pdt.second == 0);

  atc_plain_date_time_from_unix_seconds(&pdt, 946782245L);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2000);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 2);
  ACU_ASSERT(pdt.hour == 3);
  ACU_ASSERT(pdt.minute == 4);
  ACU_ASSERT(pdt.second == 5);

  atc_plain_date_time_from_unix_seconds(&pdt, 4102542245L);
  ACU_ASSERT(!atc_plain_date_time_is_error(&pdt));
  ACU_ASSERT(pdt.year == 2100);
  ACU_ASSERT(pdt.month == 1);
  ACU_ASSERT(pdt.day == 2);
  ACU_ASSERT(pdt.hour == 3);
  ACU_ASSERT(pdt.minute == 4);
  ACU_ASSERT(pdt.second == 5);
}

//---------------------------------------------------------------------------

ACU_TEST(test_plain_date_time_to_unix_seconds_invalid)
{
  AtcPlainDateTime pdt = {0, 1, 2, 3, 4, 5}; // 0000-01-02 03:04:05
  int64_t seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);

  pdt = (AtcPlainDateTime) {2000, 0, 2, 3, 4, 5}; // 2000-00-02 03:04:05
  seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 32, 3, 4, 5}; // 2000-01-32 03:04:05
  seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 24, 4, 5}; // 2000-01-02 24:04:05
  seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 60, 5}; // 2000-01-02 03:60:05
  seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);

  pdt = (AtcPlainDateTime) {2000, 1, 2, 3, 4, 60}; // 2000-01-02 03:04:60
  seconds = atc_plain_date_time_to_unix_seconds(&pdt);
  ACU_ASSERT(seconds == kAtcInvalidUnixSeconds);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_plain_date_time_errors);
  ACU_RUN_TEST(test_plain_date_time_is_valid_false);

  ACU_RUN_TEST(test_plain_date_time_to_epoch_seconds_epoch2000);
  ACU_RUN_TEST(test_plain_date_time_from_epoch_seconds_epoch2000);

  ACU_RUN_TEST(test_plain_date_time_to_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_plain_date_time_from_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_plain_date_time_to_epoch_seconds_invalid);

  ACU_RUN_TEST(test_plain_date_time_to_unix_seconds);
  ACU_RUN_TEST(test_plain_date_time_from_unix_seconds);
  ACU_RUN_TEST(test_plain_date_time_to_unix_seconds_invalid);
  ACU_SUMMARY();
}
