#include <string.h>
#include <acetimec.h>
#include "acunit.h"

ACU_TEST(test_atc_zoned_extra_from_epoch_seconds_invalid)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  struct AtcZonedExtra zet;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  bool status = atc_zoned_extra_from_epoch_seconds(
    &processing,
    &kAtcZoneEtc_UTC,
    epoch_seconds,
    &zet);
  ACU_ASSERT(status == false);

  ACU_PASS();
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_fall_back)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // Start our sampling at 01:29:00-07:00, which is 31 minutes before the DST
  // fall-back.
  struct AtcOffsetDateTime odt = { 2022, 11, 6, 1, 29, 0, 0 /*fold*/, -7*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  struct AtcZonedExtra zet;
  bool status = atc_zoned_extra_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zet);
  ACU_ASSERT(status == true);
  ACU_ASSERT(-7*60 == zet.utc_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);

  // Go forward an hour. Should be 01:29:00-08:00.
  epoch_seconds += 3600;
  status = atc_zoned_extra_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zet);
  ACU_ASSERT(status == true);
  ACU_ASSERT(-8*60 == zet.utc_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);

  ACU_PASS();
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_spring_forward)
{
  struct AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // Start our sampling at 01:29:00-08:00, which is 31 minutes before the DST
  // spring forward.
  struct AtcOffsetDateTime odt = { 2022, 3, 13, 1, 29, 0, 0 /*fold*/, -8*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  struct AtcZonedExtra zet;
  bool status = atc_zoned_extra_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zet);
  ACU_ASSERT(status == true);
  ACU_ASSERT(-8*60 == zet.utc_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  status = atc_zoned_extra_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zet);
  ACU_ASSERT(status == true);
  ACU_ASSERT(-7*60 == zet.utc_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_zoned_extra_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_fall_back);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_spring_forward);
  ACU_SUMMARY();
}
