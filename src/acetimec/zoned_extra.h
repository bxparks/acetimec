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

/** Forward declaration for AtcPlainDateTime. */
typedef struct AtcPlainDateTime AtcPlainDateTime;

/**
 * Extra information about a given time zone at a specified epoch seconds.
 */
typedef struct AtcZonedExtra {
  /**
   * Describes how the lookup from PlainDateTime or epoch_seconds was resolved,
   * as determined by the type of fold and the disambiguate parameter. This
   * parameter is identical to the AtcZonedDatetime.resolved field. The values
   * are defined by the kAtcResolvedXxx constants.
   */
  uint8_t resolved;

  /** abbreviation (e.g. PST, PDT) */
  char abbrev[kAtcAbbrevSize];

  /** STD offset */
  int32_t std_offset_seconds;

  /** DST offset */
  int32_t dst_offset_seconds;

  /** STD offset of the requested PlainDateTime or epoch_seconds */
  int32_t req_std_offset_seconds;

  /** DST offset of the requested PlainDateTime or epoch_seconds */
  int32_t req_dst_offset_seconds;
} AtcZonedExtra;

/**
 * Set the given AtcZonedExtra to its error state, AtcZonedExtra.resolved ==
 * kAtcResolvedError.
 */
void atc_zoned_extra_set_error(AtcZonedExtra *extra);

/**
 * Return true if AtcZonedExtra is an error indicated by (resolved ==
 * kResolvedError).
 */
bool atc_zoned_extra_is_error(const AtcZonedExtra *extra);

/**
 * Extract the extra zone information at given epoch seconds.
 * Returns error status in `extra->resolved`.
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
 * Returns `extra->resolved == kAtcResolvedError` if not found.
 */
void atc_zoned_extra_from_unix_seconds(
    AtcZonedExtra *extra,
    int64_t unix_seconds,
    const AtcTimeZone *tz);

/**
 * Extract the extra zone information at given PlainDateTime.
 * Returns `extra->resolved == kAtcResolvedError` if not found.
 */
void atc_zoned_extra_from_plain_date_time(
    AtcZonedExtra *extra,
    const AtcPlainDateTime *pdt,
    const AtcTimeZone *tz,
    uint8_t disambiguate);

#ifdef __cplusplus
}
#endif

#endif
