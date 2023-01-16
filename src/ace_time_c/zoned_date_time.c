/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date.h"
#include "local_date_time.h"
#include "zone_processor.h"
#include "zoned_date_time.h"
#include "zone_info_utils.h"

int8_t atc_zoned_date_time_from_epoch_seconds(
    AtcZonedDateTime *zdt,
    atc_time_t epoch_seconds,
    AtcTimeZone tz)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  zdt->tz = tz;
  return atc_processor_offset_date_time_from_epoch_seconds(
      tz.zone_processor,
      tz.zone_info,
      epoch_seconds,
      // ZonedDateTime memory layout must be same as OffsetDateTime.
      (AtcOffsetDateTime *) zdt);
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
    uint8_t fold,
    AtcTimeZone tz)
{
  zdt->tz = tz;
  return atc_processor_offset_date_time_from_local_date_time(
      tz.zone_processor,
      tz.zone_info,
      ldt,
      fold,
      // ZonedDateTime memory layout must be same as OffsetDateTime.
      (AtcOffsetDateTime *) zdt);
}

int8_t atc_zoned_date_time_convert(
    const AtcZonedDateTime *src,
    AtcTimeZone dst_tz,
    AtcZonedDateTime *dst)
{
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(src);
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;
  return atc_zoned_date_time_from_epoch_seconds(dst, epoch_seconds, dst_tz);
}

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

  return atc_processor_offset_date_time_from_local_date_time(
      zdt->tz.zone_processor,
      zdt->tz.zone_info,
      &ldt,
      zdt->fold,
      // ZonedDateTime memory layout must be same as OffsetDateTime.
      (AtcOffsetDateTime *) zdt);
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
