/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date_time.h"
#include "zone_processing.h"
#include "zoned_date_time.h"
#include "zone_info_utils.h"

static void atc_zoned_date_time_set_error(struct AtcZonedDateTime *zdt)
{
  zdt->zone_info = NULL;
}

int8_t atc_zoned_date_time_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedDateTime *zdt)
{
  atc_zoned_date_time_set_error(zdt);
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  zdt->zone_info = zone_info;
  return atc_processing_offset_date_time_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      (struct AtcOffsetDateTime *) zdt);
}

atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const struct AtcZonedDateTime *zdt)
{
  return atc_offset_date_time_to_epoch_seconds(
      (const struct AtcOffsetDateTime*) zdt);
}

int8_t atc_zoned_date_time_from_local_date_time(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    const struct AtcLocalDateTime *ldt,
    uint8_t fold,
    struct AtcZonedDateTime *zdt)
{
  zdt->zone_info = zone_info;
  return atc_processing_offset_date_time_from_local_date_time(
      processing,
      zone_info,
      ldt,
      fold,
      (struct AtcOffsetDateTime *) zdt);
}

int8_t atc_zoned_date_time_normalize(
    struct AtcZoneProcessing *processing,
    struct AtcZonedDateTime *zdt)
{
  // Copy the date/time components.
  struct AtcLocalDateTime ldt;
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
      (struct AtcOffsetDateTime *) zdt);
}

void atc_zoned_date_time_print(
    struct AtcStringBuffer *sb,
    const struct AtcZonedDateTime *zdt)
{
  atc_offset_date_time_print(sb, (const struct AtcOffsetDateTime *) zdt);
  atc_print_char(sb, '[');
  atc_print_string(sb, atc_zone_info_zone_name(zdt->zone_info));
  atc_print_char(sb, ']');
}
