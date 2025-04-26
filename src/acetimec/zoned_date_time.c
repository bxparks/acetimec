/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "epoch.h" // atc_epoch_seconds_from_unix_seconds()
#include "local_date.h"
#include "local_date_time.h"
#include "zone_processor.h"
#include "offset_date_time.h"
#include "zoned_date_time.h"

void atc_zoned_date_time_set_error(AtcZonedDateTime *zdt)
{
  zdt->month = 0; // year 0 is valid, so can't use year field
}

bool atc_zoned_date_time_is_error(const AtcZonedDateTime *zdt)
{
  return zdt->month == 0;
}

atc_time_t atc_zoned_date_time_to_epoch_seconds(const AtcZonedDateTime *zdt)
{
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  return atc_offset_date_time_to_epoch_seconds((const AtcOffsetDateTime*) zdt);
}

void atc_zoned_date_time_from_epoch_seconds(
    AtcZonedDateTime *zdt,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    atc_zoned_date_time_set_error(zdt);
    return;
  }

  zdt->tz = *tz;
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  atc_time_zone_offset_date_time_from_epoch_seconds(
      tz, epoch_seconds, (AtcOffsetDateTime *) zdt);
}

int64_t atc_zoned_date_time_to_unix_seconds(const AtcZonedDateTime *zdt)
{
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  return atc_offset_date_time_to_unix_seconds((const AtcOffsetDateTime*) zdt);
}

void atc_zoned_date_time_from_unix_seconds(
    AtcZonedDateTime *zdt,
    int64_t unix_seconds,
    const AtcTimeZone *tz)
{
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    atc_zoned_date_time_set_error(zdt);
    return;
  }
  atc_time_t epoch_seconds = atc_epoch_seconds_from_unix_seconds(unix_seconds);
  atc_zoned_date_time_from_epoch_seconds(zdt, epoch_seconds, tz);
}

void atc_zoned_date_time_from_local_date_time(
    AtcZonedDateTime *zdt,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz,
    uint8_t disambiguate)
{
  zdt->tz = *tz;
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  atc_time_zone_offset_date_time_from_local_date_time(
      tz, ldt, disambiguate, (AtcOffsetDateTime *) zdt);
}

void atc_zoned_date_time_convert(
    const AtcZonedDateTime *from,
    const AtcTimeZone *to_tz,
    AtcZonedDateTime *to)
{
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(from);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    atc_zoned_date_time_set_error(to);
    return;
  }
  atc_zoned_date_time_from_epoch_seconds(to, epoch_seconds, to_tz);
}

// The current implementation looks up the LocalDateTime using
// atc_time_zone_offset_date_time_from_local_date_time().
//
// An alternative implementation is to convert zdt into epoch_seconds, then call
// atc_time_zone_offset_date_time_from_epoch_seconds() instead. This uses the
// offset_seconds.
//
// It's not clear which implementation is better.
//
void atc_zoned_date_time_normalize(
  AtcZonedDateTime *zdt,
  uint8_t disambiguate)
{
  // Copy the date/time components.
  AtcLocalDateTime ldt;
  ldt.year = zdt->year;
  ldt.month = zdt->month;
  ldt.day = zdt->day;
  ldt.hour = zdt->hour;
  ldt.minute = zdt->minute;
  ldt.second = zdt->second;

  // ZonedDateTime memory layout must be same as OffsetDateTime.
  atc_time_zone_offset_date_time_from_local_date_time(
      &zdt->tz, &ldt, disambiguate, (AtcOffsetDateTime *) zdt);
}

void atc_zoned_date_time_print(
    AtcStringBuffer *sb,
    const AtcZonedDateTime *zdt)
{
  atc_offset_date_time_print(sb, (const AtcOffsetDateTime *) zdt);
  atc_print_char(sb, '[');
  atc_time_zone_print(sb, &zdt->tz);
  atc_print_char(sb, ']');
}
