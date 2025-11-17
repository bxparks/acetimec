#include <string.h>
#include <acetimec.h>
#include <acunit.h>

ACU_TEST(test_zoned_extra_sizeof) {
  ACU_ASSERT((int)sizeof(AtcZonedExtra) == 28); // assuming 64-bit machine
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

ACU_TEST(test_zoned_extra_from_epoch_seconds_in_overlap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-07:00 within the overlap.
  AtcOffsetDateTime odt = {2022, 11, 6, 1, 29, 0, 0 /*resolved*/, -7*3600};
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra extra;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcResolvedUnique == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // Go forward an hour to 01:29:00-08:00.
  epoch_seconds += 3600;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcResolvedUnique == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_epoch_seconds_in_gap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-08:00, which is 31 seconds before the DST
  // spring forward.
  AtcOffsetDateTime odt = {2022, 3, 13, 1, 29, 0, 0 /*resolved*/, -8*3600};
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedExtra extra;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcResolvedUnique == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  atc_zoned_extra_from_epoch_seconds(&extra, epoch_seconds, &tz);
  ACU_ASSERT(kAtcResolvedUnique == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);
}

ACU_TEST(test_zoned_extra_from_plain_date_time_in_overlap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00 which is within the overlap.
  AtcPlainDateTime pdt = {2022, 11, 6, 1, 29, 0};

  // For kAtcDisambiguateCompatible, the earlier time is selected to get
  // 01:29:00-07:00.
  AtcZonedExtra extra;
  atc_zoned_extra_from_plain_date_time(
      &extra, &pdt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(kAtcResolvedOverlapEarlier == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // For kAtcDisambiguateReversed, the later time is selected to get
  // 01:19:00-08:00.
  atc_zoned_extra_from_plain_date_time(
      &extra, &pdt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(kAtcResolvedOverlapLater == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") == 0);
}

ACU_TEST(test_zoned_extra_from_plain_date_time_in_gap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcPlainDateTime pdt = {2022, 3, 13, 2, 29, 0};

  // Start our sampling at 02:29:00 which occurs in the gap. Select the later
  // time which normalizes to 03:29:00-07:00.
  AtcZonedExtra extra;
  atc_zoned_extra_from_plain_date_time(
      &extra, &pdt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(kAtcResolvedGapLater == extra.resolved);
  ACU_ASSERT(-8*3600 == extra.std_offset_seconds);
  ACU_ASSERT(1*3600 == extra.dst_offset_seconds);
  ACU_ASSERT(-8*3600 == extra.req_std_offset_seconds);
  ACU_ASSERT(0*3600 == extra.req_dst_offset_seconds);
  ACU_ASSERT(strcmp(extra.abbrev, "PDT") == 0);

  // Check that selecting the earlier time normalizes to 01:29:00-08:00.
  atc_zoned_extra_from_plain_date_time(
      &extra, &pdt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(kAtcResolvedGapEarlier == extra.resolved);
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
  ACU_RUN_TEST(test_zoned_extra_sizeof);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_unix_seconds_invalid);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_in_overlap);
  ACU_RUN_TEST(test_zoned_extra_from_epoch_seconds_in_gap);
  ACU_RUN_TEST(test_zoned_extra_from_plain_date_time_in_overlap);
  ACU_RUN_TEST(test_zoned_extra_from_plain_date_time_in_gap);
  ACU_SUMMARY();
}
