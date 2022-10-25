#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_offset_date_time_to_epoch_seconds)
{
  // smallest valid offset_date_time
  AtcOffsetDateTime odt = {
    1931, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MIN + 1 == atc_offset_date_time_to_epoch_seconds(&odt));

  // offset_date_time at exactly epoch_seconds==0
  odt = (AtcOffsetDateTime) {
    2000, 1, 1, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(0 == atc_offset_date_time_to_epoch_seconds(&odt));

  // one day later
  odt = (AtcOffsetDateTime) {
    2000, 1, 2, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(86400 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest value using Unix Epoch
  odt = (AtcOffsetDateTime) {
    2038, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(1200798847 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest valid offset_date_time given 32-bit epoch_seconds
  odt = (AtcOffsetDateTime) {
    2068, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MAX == atc_offset_date_time_to_epoch_seconds(&odt));
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_with_offset)
{
  uint16_t offset_minutes = 1;
  AtcOffsetDateTime odt = {
    2000, 1, 1, 0, 0, 0, 0 /*fold*/, offset_minutes /*offset*/
  };
  ACU_ASSERT(offset_minutes * -60
      == atc_offset_date_time_to_epoch_seconds(&odt));
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_invalid)
{
  AtcOffsetDateTime odt = {
    kAtcInvalidYear, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(kAtcInvalidEpochSeconds
      == atc_offset_date_time_to_epoch_seconds(&odt));
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_to_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_local_epoch_year();
  atc_set_local_epoch_year(2050);

  // smallest valid offset_date_time
  AtcOffsetDateTime odt = {
    1981, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MIN + 1 == atc_offset_date_time_to_epoch_seconds(&odt));

  // offset_date_time at exactly epoch_seconds==0
  odt = (AtcOffsetDateTime) {
    2050, 1, 1, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(0 == atc_offset_date_time_to_epoch_seconds(&odt));

  // one day later
  odt = (AtcOffsetDateTime) {
    2050, 1, 2, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(86400 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest valid offset_date_time given 32-bit epoch_seconds
  odt = (AtcOffsetDateTime) {
    2118, 1, 20, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MAX == atc_offset_date_time_to_epoch_seconds(&odt));

  atc_set_local_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_from_epoch_seconds_invalid)
{
  AtcOffsetDateTime odt;
  int16_t total_offset_minutes = 0;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  int8_t err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, total_offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrGeneric);
}

ACU_TEST(test_offset_date_time_from_epoch_seconds)
{
  AtcOffsetDateTime odt;
  int8_t err;
  int16_t offset_minutes;
  atc_time_t epoch_seconds;

  offset_minutes = 0;
  epoch_seconds = 0; // 00:00:00
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 2000);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  offset_minutes = 0;
  epoch_seconds = 3661; // 01:01:01 later
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 2000);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 1);
  ACU_ASSERT(odt.minute == 1);
  ACU_ASSERT(odt.second == 1);

  offset_minutes = -8*60; // UTC-08:00
  epoch_seconds = 0; // 00:00:00
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 1999);
  ACU_ASSERT(odt.month == 12);
  ACU_ASSERT(odt.day == 31);
  ACU_ASSERT(odt.hour == 16);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);
}

//---------------------------------------------------------------------------

ACU_TEST(test_offset_date_time_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_local_epoch_year();
  atc_set_local_epoch_year(2050);

  AtcOffsetDateTime odt;
  int8_t err;
  int16_t offset_minutes;
  atc_time_t epoch_seconds;

  offset_minutes = 0;
  epoch_seconds = 0; // 00:00:00
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 2050);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  offset_minutes = 0;
  epoch_seconds = 3661; // 01:01:01 later
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 2050);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 1);
  ACU_ASSERT(odt.minute == 1);
  ACU_ASSERT(odt.second == 1);

  offset_minutes = -8*60; // UTC-08:00
  epoch_seconds = 0; // 00:00:00
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, &odt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(odt.year == 2049);
  ACU_ASSERT(odt.month == 12);
  ACU_ASSERT(odt.day == 31);
  ACU_ASSERT(odt.hour == 16);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);

  atc_set_local_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_with_offset);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_invalid);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds);
  ACU_RUN_TEST(test_offset_date_time_from_epoch_seconds_epoch2050);
  ACU_SUMMARY();
}
