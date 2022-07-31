/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date_time.h"
#include "zone_processing.h"
#include "zoned_date_time.h"

void atc_zoned_date_time_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedDateTime *zdt)
{
  atc_processing_init_for_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds);
  atc_processing_offset_date_time_from_epoch_seconds(
      processing,
      epoch_seconds,
      (struct AtcOffsetDateTime *) zdt);
  zdt->zone_info = zone_info;
}

atc_time_t atc_zoned_date_time_to_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZonedDateTime *zdt)
{
  atc_processing_init_for_year(
      processing,
      zdt->zone_info,
      zdt->year);
  return atc_processing_local_date_time_to_epoch_seconds(
      processing,
      (const struct AtcLocalDateTime *) zdt);
}
