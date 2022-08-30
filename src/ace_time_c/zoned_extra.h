/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONED_EXTRA_H
#define ACE_TIME_C_ZONED_EXTRA_H

#include <stdint.h>
#include "common.h"
#include "zone_info.h"
#include "zone_processing.h"
#include "transition.h" // kAtcAbbrevSize

/**
 * Extra information about a given time zone at a specified epoch seconds.
 * Should be identical to AtcTransitionInfo.
 */
struct AtcZonedExtra {
  /** STD offset */
  int16_t std_offset_minutes;
  /** DST offset */
  int16_t dst_offset_minutes;
  /** abbreviation (e.g. PST, PDT) */
  char abbrev[kAtcAbbrevSize];
};

/**
 * Extract the extra zone information at given epoch_seconds.
 * Returns true upon success, false upon error.
 */
int8_t atc_zoned_extra_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedExtra *extra);

#endif
