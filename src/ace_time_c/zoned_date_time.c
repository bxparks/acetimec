/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "zoned_date_time.h"

void atc_calc_date_time(
    struct AtcZoneProcessing *processing,
    atc_time_t epoch_seconds,
    const struct AtcZoneInfo *zone_info,
    struct AtcZonedDateTime *zdt)
{
  (void) zdt;
  (void) epoch_seconds;
  (void) zone_info;
  (void) processing;
}

atc_time_t atc_calc_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZonedDateTime *zdt)
{
  (void) processing;
  (void) zdt;
  return 0;
}
