/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date.h"
#include "local_date_time.h"
#include "zone_processing.h"
#include "zoned_date_time.h"
#include "zone_info_utils.h"

int8_t atc_zoned_date_time_from_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcZonedDateTime *zdt)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  zdt->zone_info = zone_info;
  return atc_processing_offset_date_time_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      (AtcOffsetDateTime *) zdt);
}

atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const AtcZonedDateTime *zdt)
{
  return atc_offset_date_time_to_epoch_seconds(
      (const AtcOffsetDateTime*) zdt);
}

int8_t atc_zoned_date_time_from_local_date_time(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    const AtcLocalDateTime *ldt,
    uint8_t fold,
    AtcZonedDateTime *zdt)
{
  zdt->zone_info = zone_info;
  return atc_processing_offset_date_time_from_local_date_time(
      processing,
      zone_info,
      ldt,
      fold,
      (AtcOffsetDateTime *) zdt);
}

int8_t atc_zoned_date_time_convert(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    const AtcZonedDateTime *src,
    AtcZonedDateTime *dst)
{
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(src);
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;
  return atc_zoned_date_time_from_epoch_seconds(
      processing, zone_info, epoch_seconds, dst);
}

int8_t atc_zoned_date_time_normalize(
    AtcZoneProcessing *processing,
    AtcZonedDateTime *zdt)
{
  // Copy the date/time components.
  AtcLocalDateTime ldt;
  ldt.year = zdt->year;
  ldt.month = zdt->month;
  ldt.day = zdt->day;
  ldt.hour = zdt->hour;
  ldt.minute = zdt->minute;
  ldt.second = zdt->second;

  return atc_processing_offset_date_time_from_local_date_time(
      processing,
      zdt->zone_info,
      &ldt,
      zdt->fold,
      (AtcOffsetDateTime *) zdt);
}

void atc_zoned_date_time_print(
    AtcStringBuffer *sb,
    const AtcZonedDateTime *zdt)
{
  atc_offset_date_time_print(sb, (const AtcOffsetDateTime *) zdt);
  atc_print_char(sb, '[');
  atc_print_string(sb, atc_zone_info_zone_name(zdt->zone_info));
  atc_print_char(sb, ']');
}
