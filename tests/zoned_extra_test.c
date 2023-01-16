#include <string.h>
#include <acetimec.h>
#include <acunit.h>

ACU_TEST(test_atc_zoned_extra_types_equal_find_result_types)
{
  ACU_ASSERT((int)kAtcZonedExtraNotFound == (int)kAtcFindResultNotFound);
  ACU_ASSERT((int)kAtcZonedExtraExact == (int)kAtcFindResultExact);
  ACU_ASSERT((int)kAtcZonedExtraGap == (int)kAtcFindResultGap);
  ACU_ASSERT((int)kAtcZonedExtraOverlap == (int)kAtcFindResultOverlap);
}

ACU_TEST(test_atc_zoned_extra_from_epoch_seconds_invalid)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcZoneEtc_UTC, &processor};

  AtcZonedExtra zet;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  int8_t err = atc_zoned_extra_from_epoch_seconds(
    &zet, epoch_seconds, tz);
  ACU_ASSERT(err == kAtcErrGeneric);
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_fall_back)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-07:00, which is 31 minutes before the DST
  // fall-back.
  AtcOffsetDateTime odt = { 2022, 11, 6, 1, 29, 0, 0 /*fold*/, -7*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra zet;
  int8_t err = atc_zoned_extra_from_epoch_seconds(&zet, epoch_seconds, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraOverlap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(1*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);

  // Go forward an hour. Should be 01:29:00-08:00.
  epoch_seconds += 3600;
  err = atc_zoned_extra_from_epoch_seconds(&zet, epoch_seconds, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraOverlap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(0*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_spring_forward)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-08:00, which is 31 minutes before the DST
  // spring forward.
  AtcOffsetDateTime odt = { 2022, 3, 13, 1, 29, 0, 0 /*fold*/, -8*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra zet;
  int8_t err = atc_zoned_extra_from_epoch_seconds(&zet, epoch_seconds, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraExact == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(0*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  err = atc_zoned_extra_from_epoch_seconds(&zet, epoch_seconds, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraExact == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(1*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);
}

ACU_TEST(test_zoned_extra_from_local_date_time_fall_back)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00(fold=0), which is 31 minutes before the DST
  // fall-back, and occurs within an overlap.
  AtcLocalDateTime ldt = {2022, 11, 6, 1, 29, 0, 0 /*fold*/};

  AtcZonedExtra zet;
  int8_t err = atc_zoned_extra_from_local_date_time(&zet, &ldt, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraOverlap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(1*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);

  // For fold=1, the second transition is selected.
  ldt.fold = 1;
  err = atc_zoned_extra_from_local_date_time(&zet, &ldt, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraOverlap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(0*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_local_date_time_spring_forward)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &processor};

  AtcLocalDateTime ldt = {2022, 3, 13, 2, 29, 0, 0 /*fold*/};

  // Start our sampling at 02:29:00(fold=0) which occurs in the gap, uses the
  // first transition, and normalizes to 03:29:00-07:00.
  AtcZonedExtra zet;
  int8_t err = atc_zoned_extra_from_local_date_time(&zet, &ldt, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraGap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(1*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(0*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PDT") == 0);

  // For fold=1, use the second transition, and normalize to 01:29:00-08:00.
  ldt.fold = 1;
  err = atc_zoned_extra_from_local_date_time(&zet, &ldt, tz);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(kAtcZonedExtraGap == zet.type);
  ACU_ASSERT(-8*60 == zet.std_offset_minutes);
  ACU_ASSERT(0*60 == zet.dst_offset_minutes);
  ACU_ASSERT(-8*60 == zet.req_std_offset_minutes);
  ACU_ASSERT(1*60 == zet.req_dst_offset_minutes);
  ACU_ASSERT(strcmp(zet.abbrev, "PST") == 0);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_zoned_extra_types_equal_find_result_types);
  ACU_RUN_TEST(test_atc_zoned_extra_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_fall_back);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_spring_forward);
  ACU_RUN_TEST(test_zoned_extra_from_local_date_time_fall_back);
  ACU_RUN_TEST(test_zoned_extra_from_local_date_time_spring_forward);
  ACU_SUMMARY();
}
