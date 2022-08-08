#include "acunit.h"
#include <acetimec.h>

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
  printf("***zdt.offset_minutes=%d\n", zdt.offset_minutes);
  ACU_ASSERT(zdt.offset_minutes == -8*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_before_dst)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

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
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_normalize_in_gap)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

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
  //ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  //ACU_ASSERT(zdt.fold == 0);
  //ACU_ASSERT(zdt.offset_minutes == -7*60);

  ACU_PASS();
}

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

  /*
  printf("***zdt.year=%d\n", zdt.year);
  ACU_ASSERT(zdt.year == 1999);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  */

  ACU_PASS();
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_invalid)
{
  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_zoned_date_time_normalize);
  ACU_RUN_TEST(test_zoned_date_time_normalize_before_dst);
  ACU_RUN_TEST(test_zoned_date_time_normalize_in_gap);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_invalid);
  ACU_SUMMARY();
}
