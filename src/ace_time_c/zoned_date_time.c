/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "zoned_date_time.h"

void atc_to_date_time_tz(
    struct AtcZonedDateTime *dt,
    atc_time_t epoch_seconds,
    struct AtcZoneInfo *zone_info,
    struct AtcZoneProcessing *processing)
{
  (void) dt;
  (void) epoch_seconds;
  (void) zone_info;
  (void) processing;
}

/** Convert struct AtcZonedDateTime to epoch seconds. */
atc_time_t atc_to_epoch_seconds_tz(
    struct AtcZonedDateTime *dt,
    struct AtcZoneProcessing *processing)
{
  (void) dt;
  (void) processing;
  return 0;
}
