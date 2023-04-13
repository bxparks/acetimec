/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file date_tuple.h
 *
 * An internal version of LocalDateTime that keeps track of the 'w', 's', or 'u'
 * suffixes from the TZDB raw files.
 */

#ifndef ACE_TIME_C_DATE_TUPLE_H
#define ACE_TIME_C_DATE_TUPLE_H

/**
 * An internal simplified version of the AtcDateTime class that uses the
 * (year, month, day, seconds, suffix) fields.
 *
 * TODO: We only need about 24-bits (3-bytes) the seconds field. It should be
 * possible to reduce this struct by 1-byte so that the object fits entirely
 * within 8-bytes, a multiple of 4 or 8 bytes which reduces memory consumption
 * on 32-bit and 64-bit processors.
 */
typedef struct AtcDateTuple {
  /** [0,10000] */
  int16_t year;

  /** [1-12] */
  uint8_t month;

  /** [1-31] */
  uint8_t day;

  /** negative values allowed */
  int32_t seconds;

  /** kAtcSuffixS, kAtcSuffixW, kAtcSuffixU */
  uint8_t suffix;
} AtcDateTuple;

/**
 * The result of comparing a DateTuple with another. This is also used to
 * compare a Transition to the time interval of the corresponding
 * AtcMatchingEra.
 */
enum {
  kAtcCompareFarPast, // 0
  kAtcComparePrior, // 1
  kAtcCompareExactMatch, // 2
  kAtcCompareWithinMatch, // 3
  kAtcCompareFarFuture, // 4
};

/** Compare a to b, ignoring the suffix. */
int8_t atc_date_tuple_compare(
    const AtcDateTuple *a,
    const AtcDateTuple *b);

/** Return (a - b) in number of seconds, ignoring the suffix. */
atc_time_t atc_date_tuple_subtract(
    const AtcDateTuple *a,
    const AtcDateTuple *b);

/** Normalize AtcDateTuple::seconds if its magnitude is more than 24 hours. */
void atc_date_tuple_normalize(AtcDateTuple *dt);

/**
 * Convert the given 'tt', offsetMinutes, and deltaMinutes into the 'w', 's'
 * and 'u' versions of the AtcDateTuple. It is allowed for 'ttw' to be an alias
 * of 'tt'.
 */
void atc_date_tuple_expand(
    const AtcDateTuple *tt,
    int32_t offset_seconds,
    int32_t delta_seconds,
    AtcDateTuple *ttw,
    AtcDateTuple *tts,
    AtcDateTuple *ttu);

/**
 * Compare the given 't' with the interval defined by [start, until). The
 * comparison is fuzzy, with a slop of about one month so that we can ignore the
 * day and seconds fields.
 *
 * The following values are returned:
 *
 *  * kAtcComparePrior if 't' is less than 'start' by at least one month,
 *  * kAtcCompareExactMatch is never returned.
 *  * kAtcCompareWithinMatch if 't' is within [start,until) with one month slop,
 *  * kAtcCompareFarFuture if 't' is greater than 'until' by at least one month,
 */
uint8_t atc_date_tuple_compare_fuzzy(
    const AtcDateTuple *t,
    const AtcDateTuple *start,
    const AtcDateTuple *until);

#endif
