/*
 * MIT License
 * Copyright (c) 2023 Brian T. Park
 */

#include <stdbool.h>
#include "zone_processor.h"
#include "offset_date_time.h" // AtcOffsetDateTime
#include "time_zone.h"

// Adapted from TimeZone::getOffsetDateTime(epochSeconds) from the
// AceTime library.
int8_t atc_time_zone_offset_date_time_from_epoch_seconds(
    const AtcTimeZone *tz,
    atc_time_t epoch_seconds,
    AtcOffsetDateTime *odt)
{
  AtcFindResult result;
  int8_t err = atc_processor_find_by_epoch_seconds(
      tz->zone_processor, tz->zone_info, epoch_seconds, &result);
  if (err) return err;

  if (result.type == kAtcFindResultNotFound) {
    return kAtcErrGeneric;
  }

  int16_t offset_minutes =
      result.std_offset_minutes + result.dst_offset_minutes;
  err = atc_offset_date_time_from_epoch_seconds(
      epoch_seconds, offset_minutes, odt);
  if (err) return err;

  odt->fold = result.fold;
  return kAtcErrOk;
}

// Adapted from TimeZone::getOffsetDateTime(const LocalDatetime&) from the
// AceTime library.
int8_t atc_time_zone_offset_date_time_from_local_date_time(
    const AtcTimeZone *tz,
    const AtcLocalDateTime *ldt,
    AtcOffsetDateTime *odt)
{
  AtcFindResult result;
  int8_t err = atc_processor_find_by_local_date_time(
      tz->zone_processor, tz->zone_info, ldt, &result);
  if (err) return err;
  if (result.type == kAtcFindResultNotFound) return kAtcErrGeneric;

  // Convert FindResult into OffsetDateTime using the requested offset.
  odt->year = ldt->year;
  odt->month = ldt->month;
  odt->day = ldt->day;
  odt->hour = ldt->hour;
  odt->minute = ldt->minute;
  odt->second = ldt->second;
  odt->offset_minutes =
      result.req_std_offset_minutes + result.req_dst_offset_minutes;
  odt->fold = result.fold;

  // Special process for kAtcFindResultGap: Convert to epochSeconds using the
  // reqStdOffsetMinutes and reqDstOffsetMinutes, then convert back to
  // OffsetDateTime using the target stdOffsetMinutes and
  // dstOffsetMinutes.
  if (result.type == kAtcFindResultGap) {
    atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(odt);
    int16_t target_offset =
        result.std_offset_minutes + result.dst_offset_minutes;
    atc_offset_date_time_from_epoch_seconds(epoch_seconds, target_offset, odt);
  }

  return kAtcErrOk;
}
