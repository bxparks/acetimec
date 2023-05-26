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

/** Forward declaration for AceTimeZone. */
typedef struct AtcTimeZone AtcTimeZone;

/** Forward declaration for AtcLocalDateTime. */
typedef struct AtcLocalDateTime AtcLocalDateTime;

/**
 * Values of the the AtcZonedExtra.type field. Must be identical to the
 * corresponding AtcFindResultXxx enums in zone_processor.h.
 */
enum {
  kAtcZonedExtraNotFound = 0, // rename this to kAtcZonedExtraError?
  kAtcZonedExtraExact = 1,
  kAtcZonedExtraOverlap = 2,
  kAtcZonedExtraGap = 3,
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

/** Set the given AtcZonedDateTime to its error state, i.e. NotFound. */
void atc_zoned_extra_set_error(AtcZonedExtra *extra);

/** Return true if AtcZonedExtra is an error. */
bool atc_zoned_extra_is_error(const AtcZonedExtra *extra);

/**
 * Extract the extra zone information at given epoch seconds.
 * Returns error status in `extra->type`.
 */
void atc_zoned_extra_from_epoch_seconds(
    AtcZonedExtra *extra,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz);

/**
 * Extract the extra zone information at given Unix seconds. Since this uses
 * the AtcZoneProcessor and its transition cache, the range of supported Unix
 * seconds is determined by the range of the epoch seconds in the context of the
 * current epoch year.
 *
 * Returns error status in `extra->type`.
 */
void atc_zoned_extra_from_unix_seconds(
    AtcZonedExtra *extra,
    int64_t unix_seconds,
    const AtcTimeZone *tz);

/**
 * Extract the extra zone information at given LocalDateTime.
 * Returns error status in `extra->type`.
 */
void atc_zoned_extra_from_local_date_time(
    AtcZonedExtra *extra,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz);

#ifdef __cplusplus
}
#endif

#endif
