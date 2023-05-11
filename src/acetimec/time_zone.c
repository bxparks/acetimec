/*
 * MIT License
 * Copyright (c) 2023 Brian T. Park
 */

#include <stdbool.h>
#include <string.h> // memcpy()
#include "local_date.h" // kAtcInvalidEpochSeconds
#include "../zoneinfo/zone_info_utils.h" // atc_zone_info_zone_name()
#include "zone_processor.h"
#include "offset_date_time.h" // AtcOffsetDateTime
#include "zoned_extra.h" // AtcZonedExtra
#include "time_zone.h"

// A default time zone representing UTC.
const AtcTimeZone atc_time_zone_utc = {NULL, NULL};

// Adapted from TimeZone::getOffsetDateTime(epochSeconds) from the
// AceTime library.
int8_t atc_time_zone_offset_date_time_from_epoch_seconds(
    const AtcTimeZone *tz,
    atc_time_t epoch_seconds,
    AtcOffsetDateTime *odt)
{
  int32_t offset_seconds;
  uint8_t fold;
  if (tz->zone_info) {
    atc_processor_init_for_zone_info(tz->zone_processor, tz->zone_info);

    AtcFindResult result;
    int8_t err = atc_processor_find_by_epoch_seconds(
        tz->zone_processor, epoch_seconds, &result);
    if (err) return err;
    if (result.type == kAtcFindResultNotFound) return kAtcErrGeneric;
    offset_seconds = result.std_offset_seconds + result.dst_offset_seconds;
    fold = result.fold;
  } else {
    offset_seconds = 0;
    fold = 0;
  }

  atc_offset_date_time_from_epoch_seconds(epoch_seconds, offset_seconds, odt);
  if (atc_offset_date_time_is_error(odt)) return kAtcErrGeneric;

  odt->fold = fold;
  return kAtcErrOk;
}

// Adapted from TimeZone::getOffsetDateTime(const LocalDatetime&) from the
// AceTime library.
int8_t atc_time_zone_offset_date_time_from_local_date_time(
    const AtcTimeZone *tz,
    const AtcLocalDateTime *ldt,
    AtcOffsetDateTime *odt)
{
  if (tz->zone_info) {
    atc_processor_init_for_zone_info(tz->zone_processor, tz->zone_info);

    AtcFindResult result;
    int8_t err = atc_processor_find_by_local_date_time(
        tz->zone_processor, ldt, &result);
    if (err) return err;
    if (result.type == kAtcFindResultNotFound) return kAtcErrGeneric;

    // Convert FindResult into OffsetDateTime using the requested offset.
    odt->year = ldt->year;
    odt->month = ldt->month;
    odt->day = ldt->day;
    odt->hour = ldt->hour;
    odt->minute = ldt->minute;
    odt->second = ldt->second;
    odt->offset_seconds =
        result.req_std_offset_seconds + result.req_dst_offset_seconds;
    odt->fold = result.fold;

    // Special process for kAtcFindResultGap: Convert to epoch_seconds using the
    // req_std_offset_seconds and req_dst_offset_seconds, then convert back to
    // AtcOffsetDateTime using the target's std_offset_seconds and
    // dst_offset_seconds.
    if (result.type == kAtcFindResultGap) {
      atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(odt);
      int32_t target_offset =
          result.std_offset_seconds + result.dst_offset_seconds;
      atc_offset_date_time_from_epoch_seconds(
          epoch_seconds, target_offset, odt);
    }
  } else {
    odt->year = ldt->year;
    odt->month = ldt->month;
    odt->day = ldt->day;
    odt->hour = ldt->hour;
    odt->minute = ldt->minute;
    odt->second = ldt->second;
    odt->offset_seconds = 0;
    odt->fold = 0;
  }

  return kAtcErrOk;
}

int8_t atc_time_zone_zoned_extra_from_epoch_seconds(
  const AtcTimeZone *tz,
  atc_time_t epoch_seconds,
  AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  if (tz->zone_info) {
    atc_processor_init_for_zone_info(tz->zone_processor, tz->zone_info);

    AtcFindResult result;
    int8_t err = atc_processor_find_by_epoch_seconds(
        tz->zone_processor, epoch_seconds, &result);
    if (err) return err;

    extra->type = result.type;
    extra->std_offset_seconds = result.std_offset_seconds;
    extra->dst_offset_seconds = result.dst_offset_seconds;
    extra->req_std_offset_seconds = result.req_std_offset_seconds;
    extra->req_dst_offset_seconds = result.req_dst_offset_seconds;
    memcpy(extra->abbrev, result.abbrev, kAtcAbbrevSize);
    extra->abbrev[kAtcAbbrevSize - 1] = '\0';
  } else {
    extra->type = kAtcFindResultExact;
    extra->std_offset_seconds = 0;
    extra->dst_offset_seconds = 0;
    extra->req_std_offset_seconds = 0;
    extra->req_dst_offset_seconds = 0;
    memcpy(extra->abbrev, "UTC", sizeof("UTC"));
  }

  return kAtcErrOk;
}

int8_t atc_time_zone_zoned_extra_from_local_date_time(
  const AtcTimeZone *tz,
  const AtcLocalDateTime *ldt,
  AtcZonedExtra *extra)
{
  if (tz->zone_info) {
    atc_processor_init_for_zone_info(tz->zone_processor, tz->zone_info);

    AtcFindResult result;
    int8_t err = atc_processor_find_by_local_date_time(
        tz->zone_processor, ldt, &result);
    if (err) return err;

    extra->type = result.type;
    extra->std_offset_seconds = result.std_offset_seconds;
    extra->dst_offset_seconds = result.dst_offset_seconds;
    extra->req_std_offset_seconds = result.req_std_offset_seconds;
    extra->req_dst_offset_seconds = result.req_dst_offset_seconds;
    memcpy(extra->abbrev, result.abbrev, kAtcAbbrevSize);
    extra->abbrev[kAtcAbbrevSize - 1] = '\0';
  } else {
    extra->type = kAtcFindResultExact;
    extra->std_offset_seconds = 0;
    extra->dst_offset_seconds = 0;
    extra->req_std_offset_seconds = 0;
    extra->req_dst_offset_seconds = 0;
    memcpy(extra->abbrev, "UTC", sizeof("UTC"));
  }

  return kAtcErrOk;
}

void atc_time_zone_print(const AtcTimeZone *tz, AtcStringBuffer *sb)
{
  if (tz->zone_info == NULL) {
    atc_print_string(sb, "UTC");
  } else {
    atc_print_string(sb, atc_zone_info_zone_name(tz->zone_info));
  }
}
