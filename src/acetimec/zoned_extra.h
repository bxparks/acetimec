/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zoned_extra.h
 *
 * Additional information about a time zone such as the STD offset, the DST
 * offset, and the abbreviation at a given epochSeconds.
 */

#ifndef ACE_TIME_C_ZONED_EXTRA_H
#define ACE_TIME_C_ZONED_EXTRA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "transition.h" // kAtcAbbrevSize

// forward declarations
typedef struct AtcTimeZone AtcTimeZone;
typedef struct AtcLocalDateTime AtcLocalDateTime;

/**
 * Values of the the AtcZonedExtra.type field. Should be identical to
 * AtcFindResultXxx
 */
enum {
  kAtcZonedExtraNotFound = 0,
  kAtcZonedExtraExact = 1,
  kAtcZonedExtraGap = 2,
  kAtcZonedExtraOverlap = 3,
};

/**
 * Extra information about a given time zone at a specified epoch seconds.
 */
typedef struct AtcZonedExtra {
  /** Type of match against the LocalDateTime or epoch_seconds. */
  int8_t type;

  /** STD offset */
  int32_t std_offset_seconds;

  /** DST offset */
  int32_t dst_offset_seconds;

  /** STD offset of the requested LocalDateTime or epoch_seconds */
  int32_t req_std_offset_seconds;

  /** DST offset of the requested LocalDateTime or epoch_seconds */
  int32_t req_dst_offset_seconds;

  /** abbreviation (e.g. PST, PDT) */
  char abbrev[kAtcAbbrevSize]; // TODO: Move this after 'type' to save space
} AtcZonedExtra;

/**
 * Extract the extra zone information at given epoch_seconds.
 * Returns true upon success, false upon error.
 */
int8_t atc_zoned_extra_from_epoch_seconds(
    AtcZonedExtra *extra,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz);

/**
 * Extract the extra zone information at given LocalDateTime.
 * Returns true upon success, false upon error.
 */
int8_t atc_zoned_extra_from_local_date_time(
    AtcZonedExtra *extra,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz);

#ifdef __cplusplus
}
#endif

#endif
