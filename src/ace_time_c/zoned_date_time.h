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
  int16_t offset_minutes; /* possibly ignored */
  struct AtcZoneInfo *zone_info; /* nullable, possibly ignored */
};

/** Convert epoch seconds to struct AtcZonedDateTime. */
void atc_calc_date_time(
    struct AtcZoneInfo *zone_info,
    struct AtcZoneProcessing *processing,
    atc_time_t epoch_seconds,
    struct AtcZonedDateTime *dt);

/** Convert struct AtcZonedDateTime to epoch seconds. */
atc_time_t atc_calc_epoch_seconds(
    struct AtcZoneProcessing *processing,
    struct AtcZonedDateTime *dt);

#endif
