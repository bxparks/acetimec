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
#include "common.h"

struct AtcZoneProcessing;
struct AtcZoneInfo;

/**
 * Date and time broken down into components. Similar to:
 *    * `struct tm` from the C library,
 *    * ZonedDateTime from AceTime library
 */
struct AtcZonedDateTime {
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

  /** offset_minutes [-840, 960] */
  int16_t offset_minutes;
  /** Identifies the time zone. Non-nullable. */
  const struct AtcZoneInfo *zone_info;
};

/**
 * Convert epoch seconds to struct AtcZonedDateTime using the time zone
 * identified by zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_zoned_date_time_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedDateTime *zdt);

/**
 * Convert struct AtcZonedDateTime to epoch seconds using the time zone
 * identified by the zone_info inside zdt.
 * Return kAtcInvalidEpochSeconds upon failure.
 */
atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const struct AtcZonedDateTime *zdt);

/**
 * Create zoned date time from components and given time zone.
 * Return non-zero error code upon failure.
 */
int8_t atc_zoned_date_time_from_local_date_time(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    const struct AtcLocalDateTime *ldt,
    uint8_t fold,
    struct AtcZonedDateTime *zdt);

/**
 * Normalize the date time components for given time zone.
 * Return non-zero error code upon failure.
 */
int8_t atc_zoned_date_time_normalize(
    struct AtcZoneProcessing *processing,
    struct AtcZonedDateTime *zdt);

/** Print the zoned date time in ISO 8601 format. */
void atc_zoned_date_time_print(
    struct AtcStringBuffer *sb,
    const struct AtcZonedDateTime *zdt);

#endif
