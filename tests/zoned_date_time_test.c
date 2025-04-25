#include <string.h> // strcmp
#include <acunit.h>
#include <acetimec.h>


// Much of the following tests adapted from ZonedDateTimeExtendedTest.ino in the
// AceTime library.

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_to_and_from_epoch_seconds)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 0;

  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 1999);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_to_and_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 0;

  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2049);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_unix_max)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneEtc_UTC, &processor};

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 1200798847;

  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2038);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 19);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 14);
  ACU_ASSERT(zdt.second == 7);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_invalid)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneEtc_UTC, &processor};

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(atc_zoned_date_time_is_error(&zdt));

  ACU_ASSERT(atc_zoned_date_time_to_epoch_seconds(&zdt)
      == kAtcInvalidEpochSeconds);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_fall_back)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-07:00, which is 31 seconds before overlap.
  AtcOffsetDateTime odt = {2022, 11, 6, 1, 29, 0, 0 /*resolved*/, -7*3600};
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-7*3600 == zdt.offset_seconds);
  ACU_ASSERT(kAtcResolvedUnique == zdt.resolved);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // Go forward an hour to 01:29:00-08:00, the second time this was seen.
  epoch_seconds += 3600;
  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*3600 == zdt.offset_seconds);
  ACU_ASSERT(kAtcResolvedUnique == zdt.resolved);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // Go forward another hour to 02:29:00-08:00 which occurs only once.
  epoch_seconds += 3600;
  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(2 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*3600 == zdt.offset_seconds);
  ACU_ASSERT(kAtcResolvedUnique == zdt.resolved);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_spring_forward)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // Start our sampling at 01:29:00-08:00, which is 31 seconds before the gap.
  AtcOffsetDateTime odt = {2022, 3, 13, 1, 29, 0, 0 /*resolved*/, -8*3600};
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(3 == zdt.month);
  ACU_ASSERT(13 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*3600 == zdt.offset_seconds);
  ACU_ASSERT(kAtcResolvedUnique == zdt.resolved);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(3 == zdt.month);
  ACU_ASSERT(13 == zdt.day);
  ACU_ASSERT(3 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-7*3600 == zdt.offset_seconds);
  ACU_ASSERT(kAtcResolvedUnique == zdt.resolved);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_to_and_from_unix_seconds)
{
  // Use 2050 to test Unix seconds beyond the 32-bit time_t limit.
  // And it is a valid epoch for the 'zonedbtesting' database.
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};
  AtcZonedDateTime zdt;

  // date +%s -d '2000-01-01T00:00:00Z'
  int64_t unix_seconds = 946684800L;

  // from unix seconds
  atc_zoned_date_time_from_unix_seconds(&zdt, unix_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 1999);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // to unix seconds
  int64_t seconds = atc_zoned_date_time_to_unix_seconds(&zdt);
  ACU_ASSERT(seconds == unix_seconds);

  // date +%s -d '2050-01-01T00:00:00Z'
  unix_seconds = 2524608000L;

  // from unix seconds
  atc_zoned_date_time_from_unix_seconds(&zdt, unix_seconds, &tz);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2049);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // to unix seconds
  seconds = atc_zoned_date_time_to_unix_seconds(&zdt);
  ACU_ASSERT(seconds == unix_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_to_and_from_unix_seconds_invalid)
{
  // Use 2050 to test Unix seconds beyond the 32-bit time_t limit.
  // And it is a valid epoch for the 'zonedbtesting' database.
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};
  AtcZonedDateTime zdt;

  int64_t unix_seconds = kAtcInvalidUnixSeconds;
  atc_zoned_date_time_from_unix_seconds(&zdt, unix_seconds, &tz);
  ACU_ASSERT(atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(unix_seconds == atc_zoned_date_time_to_unix_seconds(&zdt));

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_from_local_date_time)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcLocalDateTime ldt = {2000, 1, 1, 0, 0, 0};
  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2000);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  // check that input kAtcDisambiguateReversed gives identical results, only a
  // single LocalDateTime matches
  ldt = (AtcLocalDateTime) {2000, 1, 1, 0, 0, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2000);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcLocalDateTime ldt = {2050, 1, 1, 0, 0, 0};
  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2050);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  // check that kAtcDisambiguateReversed gives identical results
  ldt = (AtcLocalDateTime) {2050, 1, 1, 0, 0, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2050);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_before_gap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 01:59 should resolve to 01:59-08:00
  AtcLocalDateTime ldt = {2018, 3, 11, 1, 59, 0};
  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // check that kAtcDisambiguateReversed gives identical results
  ldt = (AtcLocalDateTime) {2018, 3, 11, 1, 59, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == &kAtcTestingZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_in_gap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 02:01 doesn't exist.
  AtcLocalDateTime ldt = {2018, 3, 11, 2, 1, 0};
  AtcZonedDateTime zdt;

  // Setting kAtcDisambiguateCompatible selects the later time, 03:01-07:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedGapLater);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // Setting kAtcDisambiguateLater also selects the later time.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedGapLater);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // Setting kAtcDisambiguateEarlier selects the earlier time, 01:01-08:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateEarlier);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedGapEarlier);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // Setting kAtcDisambiguateReversed also selects the earlier time.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedGapEarlier);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_after_gap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 03:01 should resolve to 03:01-07:00.
  AtcLocalDateTime ldt = {2018, 3, 11, 3, 1, 0};
  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // check that input kAtcDisambiguateReversed gives identical results
  ldt = (AtcLocalDateTime) {2018, 3, 11, 3, 1, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_before_overlap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 00:59 is an hour before overlap, so should return 00:59-07:00.
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = {2018, 11, 4, 0, 59, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // check that kAtcDisambiguateReversed gives identical results
  ldt = (AtcLocalDateTime) {2018, 11, 4, 0, 59, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_in_overlap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // There were two instances of 01:01
  AtcLocalDateTime ldt = {2018, 11, 4, 1, 1, 0};
  AtcZonedDateTime zdt;

  // kAtcDisambiguateCompatible selects the earlier time, 01:01-07:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedOverlapEarlier);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // kAtcDisambiguateEarlier selects the earlier time, 01:01-07:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateEarlier);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedOverlapEarlier);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // kAtcDisambiguateLater selects the later time, 01:01-08:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateLater);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedOverlapLater);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // kAtcDisambiguateReversed selects the later time, 01:01-08:00.
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedOverlapLater);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_after_overlap)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 02:01 should resolve to 02:01-08:00
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = {2018, 11, 4, 2, 1, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 2);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);

  // check that kAtcDisambiguateReversed gives identical results
  ldt = (AtcLocalDateTime) {2018, 11, 4, 2, 1, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateReversed);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 2);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdt.offset_seconds == -8*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_convert)
{
  AtcZoneProcessor los_angeles;
  AtcZoneProcessor new_york;
  atc_processor_init(&los_angeles);
  atc_processor_init(&new_york);
  AtcTimeZone tzla = {&kAtcTestingZoneAmerica_Los_Angeles, &los_angeles};
  AtcTimeZone tzny = {&kAtcTestingZoneAmerica_New_York, &new_york};

  // 2022-08-30 20:00-07:00 in LA
  AtcLocalDateTime ldt = {2022, 8, 30, 20, 0, 0};
  AtcZonedDateTime zdtla;
  atc_zoned_date_time_from_local_date_time(
      &zdtla, &ldt, &tzla, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdtla));

  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, &tzny, &zdtny);

  // 2022-08-30 23:00-04:00 in NYC
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdtny));
  ACU_ASSERT(zdtny.year == 2022);
  ACU_ASSERT(zdtny.month == 8);
  ACU_ASSERT(zdtny.day == 30);
  ACU_ASSERT(zdtny.hour == 23);
  ACU_ASSERT(zdtny.minute == 0);
  ACU_ASSERT(zdtny.second == 0);
  ACU_ASSERT(zdtny.resolved == kAtcResolvedUnique);
  ACU_ASSERT(zdtny.offset_seconds == -4*3600);
  ACU_ASSERT(zdtny.tz.zone_info == &kAtcTestingZoneAmerica_New_York);
}

ACU_TEST(test_zoned_date_time_normalize)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  // 2018-03-11 02:30:00-08:00 is in the gap. After normalization, it should be
  // the later time 03:30-07:00.
  AtcZonedDateTime zdt = {2018, 3, 11, 2, 30, 0, 0 /*resolved*/, -8*3600, tz};
  atc_zoned_date_time_normalize(&zdt, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 30);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.resolved == kAtcResolvedGapLater);
  ACU_ASSERT(zdt.offset_seconds == -7*3600);
  ACU_ASSERT(zdt.tz.zone_info == tz.zone_info);
}

ACU_TEST(test_zoned_date_time_print)
{
  AtcZoneProcessor processor;
  atc_processor_init(&processor);
  AtcTimeZone tz = {&kAtcTestingZoneAmerica_Los_Angeles, &processor};

  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = {2018, 3, 11, 2, 30, 0};
  atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &tz, kAtcDisambiguateCompatible);
  ACU_ASSERT(!atc_zoned_date_time_is_error(&zdt));

  char buf[64];
  AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 64);
  atc_zoned_date_time_print(&sb, &zdt);
  atc_buf_close(&sb);

  const char expected[] = "2018-03-11T03:30:00-07:00[America/Los_Angeles]";
  ACU_ASSERT(strcmp(sb.p, expected) == 0);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zoned_date_time_to_and_from_epoch_seconds);
  ACU_RUN_TEST(test_zoned_date_time_to_and_from_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_unix_max);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_fall_back);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_spring_forward);
  ACU_RUN_TEST(test_zoned_date_time_to_and_from_unix_seconds);
  ACU_RUN_TEST(test_zoned_date_time_to_and_from_unix_seconds_invalid);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_epoch2050);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_before_gap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_in_gap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_after_gap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_before_overlap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_in_overlap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_after_overlap);
  ACU_RUN_TEST(test_zoned_date_time_convert);
  ACU_RUN_TEST(test_zoned_date_time_normalize);
  ACU_RUN_TEST(test_zoned_date_time_print);
  ACU_SUMMARY();
}
