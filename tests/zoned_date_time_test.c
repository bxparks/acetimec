#include "acunit.h"
#include <acetimec.h>

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_from_epoch_seconds)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  struct AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 0;

  bool status = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 1999);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_unix_max)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  struct AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 1200798847;

  bool status = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneEtc_UTC,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2038);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 19);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 14);
  ACU_ASSERT(zdt.second == 7);
  ACU_ASSERT(zdt.fold == 0);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_invalid)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  struct AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  bool status = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneEtc_UTC,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(status == false);

  ACU_PASS();
}

//---------------------------------------------------------------------------

// The following tests adapted from ZonedDateTimeExtendedTest.ino in the AceTime
// library.

ACU_TEST(test_zoned_date_time_normalize)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  struct AtcZonedDateTime zdt = {
      2000, 1, 1, 0, 0, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2000);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_before_dst)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 01:59 should resolve to 01:59-08:00
  struct AtcZonedDateTime zdt = {
      2018, 3, 11, 1, 59, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_in_gap)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 02:01 doesn't exist. The expected TimeOffset in the gap is the previous
  // timeOffset, i.e. the most recent matching Transition, so this is
  // interpreted as 02:01-08:00 which gets normalized to 03:01-07:00.
  struct AtcZonedDateTime zdt = {
      2018, 3, 11, 2, 1, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_in_dst)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 03:01 should resolve to 03:01-07:00.
  struct AtcZonedDateTime zdt = {
      2018, 3, 11, 3, 1, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_before_sdt)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 00:59 is an hour before the DST->STD transition, so should return
  // 00:59-07:00.
  struct AtcZonedDateTime zdt = {
      2018, 11, 4, 0, 59, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_in_overlap)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // There were two instances of 01:01. The algorithm picks the earlier
  // Transition, by default (fold==0) so should resolve to 01:01-07:00.
  struct AtcZonedDateTime zdt = {
      2018, 11, 4, 1, 1, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_after_overlap)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // There were two instances of 01:01. The algorithm picks the earlier
  // Transition, by default (fold==0) so should resolve to 01:01-07:00.
  struct AtcZonedDateTime zdt = {
      2018, 11, 4, 1, 1, 0,
      0 /*fold*/, 0 /*offset*/,
      &kAtcZoneAmerica_Los_Angeles /* zone_info */
  };
  bool status = atc_zoned_date_time_normalize(&processing, &zdt);
  ACU_ASSERT(status == true);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_unix_max);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_date_time_normalize);
  ACU_RUN_TEST(test_zoned_date_time_normalize_before_dst);
  ACU_RUN_TEST(test_zoned_date_time_normalize_in_gap);
  ACU_RUN_TEST(test_zoned_date_time_normalize_in_dst);
  ACU_RUN_TEST(test_zoned_date_time_normalize_before_sdt);
  ACU_RUN_TEST(test_zoned_date_time_normalize_in_overlap);
  ACU_SUMMARY();
}
