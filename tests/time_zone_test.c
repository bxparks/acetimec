#include <string.h> // strcmp()
#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_atc_time_zone_print_utc)
{
  char buf[80];
  AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);

  atc_time_zone_print(&sb, &atc_time_zone_utc);
  atc_buf_close(&sb);
  ACU_ASSERT(strcmp(sb.p, "UTC") == 0);
}

ACU_TEST(test_atc_time_zone_offset_date_time_from_epoch_seconds_utc)
{
  const AtcTimeZone *tz = &atc_time_zone_utc;
  atc_time_t epoch_seconds = 0;
  AtcOffsetDateTime odt;
  atc_time_zone_offset_date_time_from_epoch_seconds(tz, epoch_seconds, &odt);

  ACU_ASSERT(! atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2050);
  ACU_ASSERT(odt.month == 1);
  ACU_ASSERT(odt.day == 1);
  ACU_ASSERT(odt.hour == 0);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);
  ACU_ASSERT(odt.offset_seconds == 0);
  ACU_ASSERT(odt.resolved == kAtcResolvedUnique);
}

ACU_TEST(test_atc_time_zone_offset_date_time_from_plain_date_time_utc)
{
  const AtcTimeZone *tz = &atc_time_zone_utc;
  AtcPlainDateTime pdt = {2023, 2, 14, 12, 32, 0};
  AtcOffsetDateTime odt;
  atc_time_zone_offset_date_time_from_plain_date_time(
      tz, &pdt, kAtcDisambiguateCompatible, &odt);

  ACU_ASSERT(! atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2023);
  ACU_ASSERT(odt.month == 2);
  ACU_ASSERT(odt.day == 14);
  ACU_ASSERT(odt.hour == 12);
  ACU_ASSERT(odt.minute == 32);
  ACU_ASSERT(odt.second == 0);
  ACU_ASSERT(odt.offset_seconds == 0);
  ACU_ASSERT(odt.resolved == kAtcResolvedUnique);
}

ACU_TEST(test_atc_time_zone_zoned_extra_from_epoch_seconds_utc)
{
  const AtcTimeZone *tz = &atc_time_zone_utc;
  atc_time_t epoch_seconds = 0;
  AtcZonedExtra extra;
  atc_time_zone_zoned_extra_from_epoch_seconds(tz, epoch_seconds, &extra);

  ACU_ASSERT(! atc_zoned_extra_is_error(&extra));
  ACU_ASSERT(extra.fold_type == kAtcFoldTypeExact);
  ACU_ASSERT(extra.std_offset_seconds == 0);
  printf("%d\n", extra.dst_offset_seconds);
  ACU_ASSERT(extra.dst_offset_seconds == 0);
  ACU_ASSERT(extra.req_std_offset_seconds == 0);
  ACU_ASSERT(extra.req_dst_offset_seconds == 0);
  ACU_ASSERT(strcmp(extra.abbrev, "UTC") ==0);
}

ACU_TEST(test_atc_time_zone_zoned_extra_from_plain_date_time_utc)
{
  const AtcTimeZone *tz = &atc_time_zone_utc;
  AtcPlainDateTime pdt = {2023, 2, 14, 12, 32, 0};
  AtcZonedExtra extra;
  atc_time_zone_zoned_extra_from_plain_date_time(
      tz, &pdt, kAtcDisambiguateCompatible, &extra);

  ACU_ASSERT(! atc_zoned_extra_is_error(&extra));
  ACU_ASSERT(extra.fold_type == kAtcFoldTypeExact);
  ACU_ASSERT(extra.std_offset_seconds == 0);
  ACU_ASSERT(extra.dst_offset_seconds == 0);
  ACU_ASSERT(extra.req_std_offset_seconds == 0);
  ACU_ASSERT(extra.req_dst_offset_seconds == 0);
  ACU_ASSERT(strcmp(extra.abbrev, "UTC") ==0);
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_time_zone_print_los_angeles)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  char buf[80];
  AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);

  atc_time_zone_print(&sb, &tz);
  atc_buf_close(&sb);
  ACU_ASSERT(strcmp(sb.p, "America/Los_Angeles") == 0);
}

ACU_TEST(test_atc_time_zone_offset_date_time_from_epoch_seconds_los_angeles)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  atc_time_t epoch_seconds = 0;
  AtcOffsetDateTime odt;
  atc_time_zone_offset_date_time_from_epoch_seconds(&tz, epoch_seconds, &odt);

  ACU_ASSERT(! atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2049);
  ACU_ASSERT(odt.month == 12);
  ACU_ASSERT(odt.day == 31);
  ACU_ASSERT(odt.hour == 16);
  ACU_ASSERT(odt.minute == 0);
  ACU_ASSERT(odt.second == 0);
  ACU_ASSERT(odt.offset_seconds == -8*3600);
  ACU_ASSERT(odt.resolved == kAtcResolvedUnique);
}

ACU_TEST(test_atc_time_zone_offset_date_time_from_plain_date_time_los_angeles)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcPlainDateTime pdt = {2023, 2, 14, 12, 32, 0};
  AtcOffsetDateTime odt;
  atc_time_zone_offset_date_time_from_plain_date_time(
      &tz, &pdt, kAtcDisambiguateCompatible, &odt);

  ACU_ASSERT(! atc_offset_date_time_is_error(&odt));
  ACU_ASSERT(odt.year == 2023);
  ACU_ASSERT(odt.month == 2);
  ACU_ASSERT(odt.day == 14);
  ACU_ASSERT(odt.hour == 12);
  ACU_ASSERT(odt.minute == 32);
  ACU_ASSERT(odt.second == 0);
  ACU_ASSERT(odt.offset_seconds == -8*3600);
  ACU_ASSERT(odt.resolved == kAtcResolvedUnique);
}

ACU_TEST(test_atc_time_zone_zoned_extra_from_epoch_seconds_los_angeles)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  atc_time_t epoch_seconds = 0;
  AtcZonedExtra extra;
  atc_time_zone_zoned_extra_from_epoch_seconds(&tz, epoch_seconds, &extra);

  ACU_ASSERT(! atc_zoned_extra_is_error(&extra));
  ACU_ASSERT(extra.fold_type == kAtcFoldTypeExact);
  ACU_ASSERT(extra.std_offset_seconds == -8*3600);
  ACU_ASSERT(extra.dst_offset_seconds == 0);
  ACU_ASSERT(extra.req_std_offset_seconds == -8*3600);
  ACU_ASSERT(extra.req_dst_offset_seconds == 0);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") ==0);
}

ACU_TEST(test_atc_time_zone_zoned_extra_from_plain_date_time_los_angeles)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcPlainDateTime pdt = {2023, 2, 14, 12, 32, 0};
  AtcZonedExtra extra;
  atc_time_zone_zoned_extra_from_plain_date_time(
      &tz, &pdt, kAtcDisambiguateCompatible, &extra);

  ACU_ASSERT(! atc_zoned_extra_is_error(&extra));
  ACU_ASSERT(extra.fold_type == kAtcFoldTypeExact);
  ACU_ASSERT(extra.std_offset_seconds == -8*3600);
  ACU_ASSERT(extra.dst_offset_seconds == 0);
  ACU_ASSERT(extra.req_std_offset_seconds == -8*3600);
  ACU_ASSERT(extra.req_dst_offset_seconds == 0);
  ACU_ASSERT(strcmp(extra.abbrev, "PST") ==0);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_time_zone_print_utc);
  ACU_RUN_TEST(test_atc_time_zone_offset_date_time_from_epoch_seconds_utc);
  ACU_RUN_TEST(test_atc_time_zone_offset_date_time_from_plain_date_time_utc);
  ACU_RUN_TEST(test_atc_time_zone_zoned_extra_from_epoch_seconds_utc);
  ACU_RUN_TEST(test_atc_time_zone_zoned_extra_from_plain_date_time_utc);

  ACU_RUN_TEST(test_atc_time_zone_print_los_angeles);
  ACU_RUN_TEST(test_atc_time_zone_offset_date_time_from_epoch_seconds_los_angeles);
  ACU_RUN_TEST(test_atc_time_zone_offset_date_time_from_plain_date_time_los_angeles);
  ACU_RUN_TEST(test_atc_time_zone_zoned_extra_from_epoch_seconds_los_angeles);
  ACU_RUN_TEST(test_atc_time_zone_zoned_extra_from_plain_date_time_los_angeles);
  ACU_SUMMARY();
}
