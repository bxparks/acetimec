#include <acunit.h>
#include <acetimec.h>


// Much of the following tests adapted from ZonedDateTimeExtendedTest.ino in the
// AceTime library.

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_from_epoch_seconds)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 0;

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 1999);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 0;

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2049);
  ACU_ASSERT(zdt.month == 12);
  ACU_ASSERT(zdt.day == 31);
  ACU_ASSERT(zdt.hour == 16);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_unix_max)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = 1200798847;

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneEtc_UTC,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2038);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 19);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 14);
  ACU_ASSERT(zdt.second == 7);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneEtc_UTC);

  atc_time_t eps = atc_zoned_date_time_to_epoch_seconds(&zdt);
  ACU_ASSERT(eps == epoch_seconds);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_invalid)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcZonedDateTime zdt;
  atc_time_t epoch_seconds = kAtcInvalidEpochSeconds;

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneEtc_UTC,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrGeneric);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_fall_back)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);
  AtcZonedDateTime zdt;

  // Start our sampling at 01:29:00-07:00, which is 31 minutes before the DST
  // fall-back.
  AtcOffsetDateTime odt = { 2022, 11, 6, 1, 29, 0, 0 /*fold*/, -7*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-7*60 == zdt.offset_minutes);
  ACU_ASSERT(0 == zdt.fold);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // Go forward an hour. Should return 01:29:00-08:00, the second time this
  // was seen, so fold should be 1.
  epoch_seconds += 3600;
  err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*60 == zdt.offset_minutes);
  ACU_ASSERT(1 == zdt.fold);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // Go forward another hour. Should return 02:29:00-08:00, which occurs only
  // once, so fold should be 0.
  epoch_seconds += 3600;
  err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(11 == zdt.month);
  ACU_ASSERT(6 == zdt.day);
  ACU_ASSERT(2 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*60 == zdt.offset_minutes);
  ACU_ASSERT(0 == zdt.fold);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_epoch_seconds_spring_forward)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);
  AtcZonedDateTime zdt;

  // Start our sampling at 01:29:00-08:00, which is 31 minutes before the DST
  // spring forward.
  AtcOffsetDateTime odt = { 2022, 3, 13, 1, 29, 0, 0 /*fold*/, -8*60 };
  atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(&odt);

  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(3 == zdt.month);
  ACU_ASSERT(13 == zdt.day);
  ACU_ASSERT(1 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-8*60 == zdt.offset_minutes);
  ACU_ASSERT(0 == zdt.fold);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // An hour later, we spring forward to 03:29:00-07:00.
  epoch_seconds += 3600;
  err = atc_zoned_date_time_from_epoch_seconds(
    &processing,
    &kAtcZoneAmerica_Los_Angeles,
    epoch_seconds,
    &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(2022 == zdt.year);
  ACU_ASSERT(3 == zdt.month);
  ACU_ASSERT(13 == zdt.day);
  ACU_ASSERT(3 == zdt.hour);
  ACU_ASSERT(29 == zdt.minute);
  ACU_ASSERT(0 == zdt.second);
  ACU_ASSERT(-7*60 == zdt.offset_minutes);
  ACU_ASSERT(0 == zdt.fold);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_from_local_date_time)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcLocalDateTime ldt = { 2000, 1, 1, 0, 0, 0 };
  AtcZonedDateTime zdt;
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2000);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2000, 1, 1, 0, 0, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2000);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  AtcLocalDateTime ldt = { 2050, 1, 1, 0, 0, 0 };
  AtcZonedDateTime zdt;
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2050);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2050, 1, 1, 0, 0, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2050);
  ACU_ASSERT(zdt.month == 1);
  ACU_ASSERT(zdt.day == 1);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 0);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
  ACU_ASSERT(8 * 60 * 60 == atc_zoned_date_time_to_epoch_seconds(&zdt));

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_before_dst)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 01:59 should resolve to 01:59-08:00
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = { 2018, 3, 11, 1, 59, 0 };
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2018, 3, 11, 1, 59, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_in_gap)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 02:01 doesn't exist.
  // Setting (fold=0) causes the first transition to be selected, which has a
  // UTC offset of -08:00, so this is interpreted as 02:01-08:00 which gets
  // normalized to 03:01-07:00.
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = { 2018, 3, 11, 2, 1, 0 };
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1); // indicate the second transition
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // Setting (fold=1) causes the second transition to be selected, which has a
  // UTC offset of -07:00, so this is interpreted as 02:01-07:00 which gets
  // normalized to 01:01-08:00.
  ldt = (AtcLocalDateTime) { 2018, 3, 11, 2, 1, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0); // indicate the first transition
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_in_dst)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 03:01 should resolve to 03:01-07:00.
  AtcLocalDateTime ldt = { 2018, 3, 11, 3, 1, 0 };
  AtcZonedDateTime zdt;
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2018, 3, 11, 3, 1, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 3);
  ACU_ASSERT(zdt.day == 11);
  ACU_ASSERT(zdt.hour == 3);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_before_sdt)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 00:59 is an hour before the DST->STD transition, so should return
  // 00:59-07:00.
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = { 2018, 11, 4, 0, 59, 0 };
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2018, 11, 4, 0, 59, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 0);
  ACU_ASSERT(zdt.minute == 59);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_in_overlap)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // There were two instances of 01:01

  // Setting (fold==0) selects the first instance, resolves to 01:01-07:00.
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = { 2018, 11, 4, 1, 1, 0 };
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -7*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // Setting (fold==1) selects the second instance, resolves to 01:01-08:00.
  ldt = (AtcLocalDateTime) { 2018, 11, 4, 1, 1, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 1);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

ACU_TEST(test_zoned_date_time_from_local_date_time_after_overlap)
{
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  // 02:01 should resolve to 02:01-08:00
  AtcZonedDateTime zdt;
  AtcLocalDateTime ldt = { 2018, 11, 4, 2, 1, 0 };
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      0 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 2);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 0);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);

  // check that fold=1 gives identical results, while preserving fold
  ldt = (AtcLocalDateTime) { 2018, 11, 4, 2, 1, 0 };
  err = atc_zoned_date_time_from_local_date_time(
      &processing,
      &kAtcZoneAmerica_Los_Angeles,
      &ldt,
      1 /*fold*/,
      &zdt);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(zdt.year == 2018);
  ACU_ASSERT(zdt.month == 11);
  ACU_ASSERT(zdt.day == 4);
  ACU_ASSERT(zdt.hour == 2);
  ACU_ASSERT(zdt.minute == 1);
  ACU_ASSERT(zdt.second == 0);
  ACU_ASSERT(zdt.fold == 1);
  ACU_ASSERT(zdt.offset_minutes == -8*60);
  ACU_ASSERT(zdt.zone_info == &kAtcZoneAmerica_Los_Angeles);
}

//---------------------------------------------------------------------------

ACU_TEST(test_zoned_date_time_convert)
{
  AtcZoneProcessing los_angeles;
  AtcZoneProcessing new_york;

  atc_processing_init(&los_angeles);
  atc_processing_init(&new_york);

  AtcLocalDateTime ldt = { 2022, 8, 30, 20, 0, 0 };
  AtcZonedDateTime ladt;
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &los_angeles, &kAtcZoneAmerica_Los_Angeles, &ldt, 0 /*fold*/, &ladt);
  ACU_ASSERT(err == kAtcErrOk);

  AtcZonedDateTime nydt;
  atc_zoned_date_time_convert(
      &new_york, &kAtcZoneAmerica_New_York, &ladt, &nydt);

  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(nydt.year == 2022);
  ACU_ASSERT(nydt.month == 8);
  ACU_ASSERT(nydt.day == 30);
  ACU_ASSERT(nydt.hour == 23);
  ACU_ASSERT(nydt.minute == 0);
  ACU_ASSERT(nydt.second == 0);
  ACU_ASSERT(nydt.fold == 0);
  ACU_ASSERT(nydt.offset_minutes == -4*60);
  ACU_ASSERT(nydt.zone_info == &kAtcZoneAmerica_New_York);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_unix_max);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_invalid);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_fall_back);
  ACU_RUN_TEST(test_zoned_date_time_from_epoch_seconds_spring_forward);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_epoch2050);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_before_dst);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_in_gap);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_in_dst);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_before_sdt);
  ACU_RUN_TEST(test_zoned_date_time_from_local_date_time_in_overlap);
  ACU_RUN_TEST(test_zoned_date_time_convert);
  ACU_SUMMARY();
}
