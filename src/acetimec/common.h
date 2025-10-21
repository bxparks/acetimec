/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file common.h
 *
 * Some common shared typedefs, constants, and functions.
 */

#ifndef ACE_TIME_C_COMMON_H
#define ACE_TIME_C_COMMON_H

#include <stddef.h> // size_t
#include <stdint.h> // INT32_MIN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The number of seconds from the current epoch. The current epoch is
 * 2050-01-01 00:00:00 UTC by default, but can be adjusted using
 * `atc_set_current_epoch_year()`.
 */
typedef int32_t atc_time_t;

enum {
  /** Error code to indicate success. */
  kAtcErrOk = 0,

  /** Error code to indicate generic failure. */
  kAtcErrGeneric = 1,
};

enum {
  /**
   * Epoch year used by the internal converter functions
   * `atc_convert_to_internal_days()` and `atc_convert_from_internal_days()` so
   * that the "internal epoch" is {year}-01-01T00:00:00. This must be a multiple
   * of 400.
   *
   * This is an internal implementation detail and should not normally be needed
   * by client applications. They should instead use
   * atc_get_current_epoch_year() and atc_set_current_epoch_year().
   */
  kAtcInternalEpochYear = 2000,

  /** Number of days from the Unix epoch (1970) to the Internal epoch (2000). */
  kAtcDaysToInternalEpochFromUnixEpoch = 10957,

  /**
   * Default epoch year of the library. This is selected so that the 32-bit
   * atc_time_t epoch seconds type is valid at least over the 100 years from
   * 2000 to 2100.
   */
  kAtcDefaultEpochYear = 2050,

  /**
   * Number of days from the Internal epoch (2000) to the default epoch (2050).
   * This is 50 years * 365 + 13 leap days.
   */
  kAtcDaysToDefaultEpochFromInternalEpoch = 365*50 + 13,

  /** Sentinel value for invalid year. */
  kAtcInvalidYear = INT16_MIN,

  /** Invalid seconds, for example, from a PlainTime. */
  kAtcInvalidSeconds = INT32_MIN,

  /** Invalid epoch days. */
  kAtcInvalidEpochDays = INT32_MIN,

  /** Invalid epoch seconds. */
  kAtcInvalidEpochSeconds = INT32_MIN,

  /** Invalid Unix days. */
  kAtcInvalidUnixDays = INT32_MIN,

  /** Invalid Unix seconds. */
  kAtcInvalidUnixSeconds = INT64_MIN,

  /**
   * Minimum year reasonablly supported by the functions in `plain_date.h`.
   * It might be more strict to make this 1, but those functions need to handle
   * yeare 0 to handle year shifts due to UTC offset.
   */
  kAtcMinYear = 0,

  /**
   * Maxium year reasonablly supported by the functions in `plain_date.h`.
   * It might be more strict to make this 9999 to prevent formatting problems
   * with more than 4 digits. But those functions need to handle a little bit of
   * slop to handle year shifts due to UTC offset.
   */
  kAtcMaxYear = 10000,
};

enum {
  /** Resolved to unique ZonedDateTime. */
  kAtcResolvedUnique = 0,

  /** In overlap and resolved to earlier time. */
  kAtcResolvedOverlapEarlier = 1,

  /** In overlap and resolved to later time. */
  kAtcResolvedOverlapLater = 2,

  /** In gap and resolved to earlier time. */
  kAtcResolvedGapEarlier = 3,

  /** In gap and resolved to later time. */
  kAtcResolvedGapLater = 4,
};

enum {
  /** Earlier for overlap, later for gap. */
  kAtcDisambiguateCompatible = 0,

  /** Always pick earlier. */
  kAtcDisambiguateEarlier = 1,

  /** Always pick later. */
  kAtcDisambiguateLater = 2,

  /** Opposite of Compatible. */
  kAtcDisambiguateReversed = 3,
};

/**
 * Copy at most dst_size characters from src to dst, while replacing all
 * occurrence of old_char with new_string. If new_string is "", then replace
 * with nothing. The resulting dst string is always NUL terminated.
 */
void atc_copy_replace_string(char *dst, size_t dst_size, const char *src,
    char old_char, const char *new_string);

/**
 * Implement the djb2 hash algorithm as described in
 * https://stackoverflow.com/questions/7666509 and
 * http://www.cse.yorku.ca/~oz/hash.html.
 */
uint32_t atc_djb2(const char *s);

/** Convert (unsigned) seconds to hour, minute, and second components. */
void atc_seconds_to_hms(
    uint32_t seconds, uint16_t *hh, uint16_t *mm, uint16_t *ss);

#ifdef __cplusplus
}
#endif

#endif
