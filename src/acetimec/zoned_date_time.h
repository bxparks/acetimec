/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zoned_date_time.h
 *
 * Functions that relate to the date time with a reference to a specific time
 * zone. For example, "2022-08-30 14:45:00-08:00 [America/Los_Angeles]".
 */

#ifndef ACE_TIME_C_ZONED_DATE_TIME_H
#define ACE_TIME_C_ZONED_DATE_TIME_H

#include <stdint.h>
#include <stdbool.h>
#include "../zoneinfo/zone_info.h"
#include "common.h"
#include "zone_processor.h"
#include "time_zone.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Date and time broken down into components. Similar to:
 *    * `struct tm` from the C library,
 *    * ZonedDateTime from AceTime library
 */
typedef struct AtcZonedDateTime {
  /** year [0,9999] */
  int16_t year;
  /** month [1,12] */
  uint8_t month;
  /** day [1,31] */
  uint8_t day;

  /** hour [0-23] */
  uint8_t hour;
  /** minute [0, 59] */
  uint8_t minute;
  /** second [0, 59] */
  uint8_t second;
  /** fold [0,1] */
  uint8_t fold;

  /** offset_seconds [-10h,+14h] roughly */
  int32_t offset_seconds;

  /** time zone. */
  AtcTimeZone tz;
} AtcZonedDateTime;

/** Set the given AtcZonedDateTime to its error state. */
void atc_zoned_date_time_set_error(AtcZonedDateTime *zdt);

/** Return true if AtcZonedDateTime is an error. */
bool atc_zoned_date_time_is_error(const AtcZonedDateTime *zdt);

/**
 * Convert AtcZonedDateTime to epoch seconds using the time zone
 * identified by the zone_info inside zdt.
 * Return kAtcInvalidEpochSeconds upon failure.
 */
atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const AtcZonedDateTime *zdt);

/**
 * Convert epoch seconds to AtcZonedDateTime using the time zone
 * identified by zone_info.
 * Return an error value for zdt upon error.
 */
void atc_zoned_date_time_from_epoch_seconds(
    AtcZonedDateTime *zdt,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz);

/**
 * Convert AtcZonedDateTime to unix seconds using the time zone
 * identified by the zone_info inside zdt.
 * Return kAtcInvalidUnixSeconds upon failure.
 */
int64_t atc_zoned_date_time_to_unix_seconds(
    const AtcZonedDateTime *zdt);

/**
 * Convert unix seconds to AtcZonedDateTime using the time zone
 * identified by zone_info.
 *
 * This implementation must convert the 64-bit Unix seconds to the 32-bit epoch
 * seconds so that the internal time zone functions can be used (i.e.
 * atc_time_zone_offset_date_time_from_epoch_seconds()). Therefore, the range of
 * valid unix_seconds is determined by the range of the 32-bit epoch seconds and
 * the current epoch year given by atc_get_current_epoch_year().
 *
 * Return an error value for zdt upon error.
 */
void atc_zoned_date_time_from_unix_seconds(
    AtcZonedDateTime *zdt,
    int64_t unix_seconds,
    const AtcTimeZone *tz);

/**
 * Create zoned date time from components and given time zone.
 * Return an error value for zdt upon error.
 */
void atc_zoned_date_time_from_local_date_time(
    AtcZonedDateTime *zdt,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz);

/**
 * Convert the source AtcZonedDateTime `from` into the destination
 * AtcZonedDateTime `to` using the new time zone `to_tz`. The `from` and `to`
 * AtcZonedDateTime objects are permitted to be the same instance.
 *
 * Return an error value for `to` upon error.
 */
void atc_zoned_date_time_convert(
    const AtcZonedDateTime *from,
    const AtcTimeZone *to_tz,
    AtcZonedDateTime *to);

/**
 * Normalize the date time components for given time zone.
 * This is useful after performing arithmetic operations on the date or time
 * components (e.g. incrementing the day by one).
 *
 * Return an error value for `zdt` upon error.
 */
void atc_zoned_date_time_normalize(AtcZonedDateTime *zdt);

/** Print the zoned date time in ISO 8601 format. */
void atc_zoned_date_time_print(
    AtcStringBuffer *sb,
    const AtcZonedDateTime *zdt);

#ifdef __cplusplus
}
#endif

#endif
