/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
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
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int16_t offset_minutes;
  const struct AtcZoneInfo *zone_info; /* non-nullable */
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
int8_t atc_zoned_date_time_from_components(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    int16_t year, uint8_t month, uint8_t day,
    uint8_t hour, uint8_t minute, uint8_t second,
    uint8_t fold,
    struct AtcZonedDateTime *zdt);

/**
 * Normalize the date time components for given time zone.
 * Return non-zero error code upon failure.
 */
int8_t atc_zoned_date_time_normalize(
    struct AtcZoneProcessing *processing,
    struct AtcZonedDateTime *zdt);

#endif
