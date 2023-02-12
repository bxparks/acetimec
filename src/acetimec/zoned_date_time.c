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

int8_t atc_zoned_date_time_from_epoch_seconds(
    AtcZonedDateTime *zdt,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  zdt->tz = *tz;
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  return atc_time_zone_offset_date_time_from_epoch_seconds(
      tz, epoch_seconds, (AtcOffsetDateTime *) zdt);
}

atc_time_t atc_zoned_date_time_to_epoch_seconds(const AtcZonedDateTime *zdt)
{
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  return atc_offset_date_time_to_epoch_seconds(
      (const AtcOffsetDateTime*) zdt);
}

int8_t atc_zoned_date_time_from_local_date_time(
    AtcZonedDateTime *zdt,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz)
{
  zdt->tz = *tz;
  // ZonedDateTime memory layout must be same as OffsetDateTime.
  return atc_time_zone_offset_date_time_from_local_date_time(
      tz, ldt, (AtcOffsetDateTime *) zdt);
}

int8_t atc_zoned_date_time_convert(
    const AtcZonedDateTime *src,
    const AtcTimeZone *dst_tz,
    AtcZonedDateTime *dst)
{
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(src);
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;
  return atc_zoned_date_time_from_epoch_seconds(dst, epoch_seconds, dst_tz);
}

// The current implementation looks up the LocalDateTime using
// atc_time_zone_offset_date_time_from_local_date_time(). This uses the fold and
// ignores the offset_minutes.
//
// An alternative implementation is to convert zdt into epoch_seconds, then call
// atc_time_zone_offset_date_time_from_epoch_seconds() instead. This uses the
// offset_minutes, and ignores the fold.
//
// It's not clear which implementation is better.
//
int8_t atc_zoned_date_time_normalize(AtcZonedDateTime *zdt)
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
  return atc_time_zone_offset_date_time_from_local_date_time(
      &zdt->tz, &ldt, (AtcOffsetDateTime *) zdt);
}

void atc_zoned_date_time_print(
    const AtcZonedDateTime *zdt,
    AtcStringBuffer *sb)
{
  atc_offset_date_time_print((const AtcOffsetDateTime *) zdt, sb);
  atc_print_char(sb, '[');
  atc_print_string(sb, atc_zone_info_zone_name(zdt->tz.zone_info));
  atc_print_char(sb, ']');
}
