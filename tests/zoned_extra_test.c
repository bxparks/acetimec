#include <string.h>
#include <acetimec.h>
#include <acunit.h>

ACU_TEST(test_zoned_extra_types_equal_find_result_types)
{
  ACU_ASSERT((int)kAtcZonedExtraNotFound == (int)kAtcFindResultNotFound);
  ACU_ASSERT((int)kAtcZonedExtraExact == (int)kAtcFindResultExact);
  ACU_ASSERT((int)kAtcZonedExtraGap == (int)kAtcFindResultGap);
  ACU_ASSERT((int)kAtcZonedExtraOverlap == (int)kAtcFindResultOverlap);
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_invalid)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneEtc_UTC, &processor};

  AtcZonedExtra extra;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(atc_zoned_extra_is_error(&extra));
}

ACU_TEST(test_zoned_extra_from_unix_seconds_invalid)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneEtc_UTC, &processor};

  AtcZonedExtra extra;
  int64_t unix_seconds = kAtcInvalidUnixSeconds;

  atc_zoned_extra_from_unix_seconds(&extra, unix_seconds, &tz);
  ACU_ASSERT(atc_zoned_extra_is_error(&extra));
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_fall_back)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-07:00, which is 31 seconds before the DST
  // fall-back.
  AtcOffsetDateTime odt = { 2022, 11, 6, 1, 29, 0, 0 /*fold*/, -7*3600 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra extra;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcZonedExtraOverlap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // Go forward an hour. Should be 01:29:00-08:00.
  epoch_seconds += 3600;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcZonedExtraOverlap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_spring_forward)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-08:00, which is 31 seconds before the DST
  // spring forward.
  AtcOffsetDateTime odt = { 2022, 3, 13, 1, 29, 0, 0 /*fold*/, -8*3600 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra extra;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcZonedExtraExact == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcZonedExtraExact == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);
}

ACU_TEST(test_zoned_extra_from_local_date_time_fall_back)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00(fold=0), which is 31 seconds before the DST
  // fall-back, and occurs within an overlap.
  AtcLocalDateTime ldt = {2022, 11, 6, 1, 29, 0, 0 /*fold*/};

  AtcZonedExtra extra;
  atc_zoned_extra_from_local_date_time(&extra, &ldt, &tz);
  ACU_ASSERT(kAtcZonedExtraOverlap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // For fold=1, the second transition is selected.
  ldt.fold = 1;
  atc_zoned_extra_from_local_date_time(&extra, &ldt, &tz);
  ACU_ASSERT(kAtcZonedExtraOverlap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_local_date_time_spring_forward)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcLocalDateTime ldt = {2022, 3, 13, 2, 29, 0, 0 /*fold*/};

  // Start our sampling at 02:29:00(fold=0) which occurs in the gap, uses the
  // first transition, and normalizes to 03:29:00-07:00.
  AtcZonedExtra extra;
  atc_zoned_extra_from_local_date_time(&extra, &ldt, &tz);
  ACU_ASSERT(kAtcZonedExtraGap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // For fold=1, use the second transition, and normalize to 01:29:00-08:00.
  ldt.fold = 1;
  atc_zoned_extra_from_local_date_time(&extra, &ldt, &tz);
  ACU_ASSERT(kAtcZonedExtraGap == extra.type);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zoned_extra_types_equal_find_result_types);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_unix_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_fall_back);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_spring_forward);
  ACU_RUN_TEST(test_zoned_extra_from_local_date_time_fall_back);
  ACU_RUN_TEST(test_zoned_extra_from_local_date_time_spring_forward);
  ACU_SUMMARY();
}
