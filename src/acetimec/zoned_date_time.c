/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "../zoneinfo/zone_info_utils.h" // atc_zone_info_zone_name()
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

void atc_zoned_date_time_from_local_date_time(
    AtcZonedDateTime *zdt,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz)
{
  zdt->tz = *tz;
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  int8_t err = atc_time_zone_offset_date_time_from_local_date_time(
      tz, ldt, (AtcOffsetDateTime *) zdt);
  if (err) {
    atc_zoned_date_time_set_error(zdt);
  }
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
// atc_time_zone_offset_date_time_from_local_date_time(). This uses the fold and
// ignores the offset_seconds.
//
// An alternative implementation is to convert zdt into epoch_seconds, then call
// atc_time_zone_offset_date_time_from_epoch_seconds() instead. This uses the
// offset_seconds, and ignores the fold.
//
// It's not clear which implementation is better.
//
void atc_zoned_date_time_normalize(AtcZonedDateTime *zdt)
{
  // Copy the date/time components.
  AtcLocalDateTime ldt;
  ldt.year = zdt->year;
  ldt.month = zdt->month;
  ldt.day = zdt->day;
  ldt.hour = zdt->hour;
  ldt.minute = zdt->minute;
  ldt.second = zdt->second;
  ldt.fold = zdt->fold;

  // ZonedDateTime memory layout must be same as OffsetDateTime.
  int8_t err = atc_time_zone_offset_date_time_from_local_date_time(
      &zdt->tz, &ldt, (AtcOffsetDateTime *) zdt);
  if (err) {
    atc_zoned_date_time_set_error(zdt);
  }
}

void atc_zoned_date_time_print(
    const AtcZonedDateTime *zdt,
    AtcStringBuffer *sb)
{
  atc_offset_date_time_print((const AtcOffsetDateTime *) zdt, sb);
  atc_print_char(sb, '[');
  atc_time_zone_print(&zdt->tz, sb);
  atc_print_char(sb, ']');
}
