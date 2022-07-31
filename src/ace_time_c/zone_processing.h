/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_PROCESSING_H
#define ACE_TIME_C_ZONE_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h" // atc_time_t
#include "local_date_time.h" // AtcLocalDateTime
#include "zone_info.h"

//---------------------------------------------------------------------------
// Conversion and accessor utilities.
//---------------------------------------------------------------------------

/**
 * Convert time code (in 15 minute increments to minutes
 * The lower 4-bits of the modifier contains the remaining 0-14 minutes.
 */
uint16_t atc_zone_info_time_code_to_minutes(
  uint8_t code, uint8_t modifier);

/** Extract the time suffix (w, s, gu) from the upper 4-bits of the modifier. */
uint8_t atc_zone_info_modifier_to_suffix(uint8_t modifier);

/**
 * Extract the actual (month, day) pair from the expression used in the TZ
 * data files of the form (onDayOfWeek >= onDayOfMonth) or (onDayOfWeek <=
 * onDayOfMonth).
 *
 * There are 4 combinations:
 *
 * @verbatim
 * onDayOfWeek=0, onDayOfMonth=(1-31): exact match
 * onDayOfWeek=1-7, onDayOfMonth=1-31: dayOfWeek>=dayOfMonth
 * onDayOfWeek=1-7, onDayOfMonth=0: last{dayOfWeek}
 * onDayOfWeek=1-7, onDayOfMonth=-(1-31): dayOfWeek<=dayOfMonth
 * @endverbatim
 *
 * Caveats: This function handles expressions which crosses month boundaries,
 * but not year boundaries (e.g. Jan to Dec of the previous year, or Dec to
 * Jan of the following year.)
 */
void atc_processing_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month,
    uint8_t *result_month,
    uint8_t *result_day);

//---------------------------------------------------------------------------
// Data structures to track ZoneEra transitions and associated info.
//---------------------------------------------------------------------------

enum {
  /**
   * Number of characters in a time zone abbreviation. 6 is the maximum
   * plus 1 for the NUL terminator.
   */
  kAtcAbbrevSize = 7,

  /** Number of transitions in the transition_storage. */
  kAtcTransitionStorageSize = 8,

  /**
   * Number of Matches. We look at the 3 years straddling the current year, plus
   * the most recent prior year, so that makes 4.
   */
  kAtcMaxMatches = 4,

  /**
   * Maximum number of interior years. For a viewing window of 14 months,
   * this will be 4. (Verify: I think this can be changed to 3.)
   */
  kAtcMaxInteriorYears = 4,
};

/** A tuple of (year_tiny, month). */
struct AtcYearMonth {
  int8_t year_tiny;
  uint8_t month;
};

/** An internal tuple of (year_tiny, month, day, minutes). */
struct AtcDateTuple {
  int8_t year_tiny; // [-127, 126], 127 will cause bugs
  uint8_t month; // [1-12]
  uint8_t day; // [1-31]
  uint8_t suffix; // kAtcSuffixS, kAtcSuffixW, kAtcSuffixU
  int16_t minutes; // negative values allowed
};

/**
 * Compare AtcDateTuple a to AtcDateTuple b, ignoring the suffix.
 * Exported for testing.
 */
int8_t atc_compare_internal_date_time(
  const struct AtcDateTuple *a,
  const struct AtcDateTuple *b);

//---------------------------------------------------------------------------
// AtcTransition and AtcTransitionStorage
//---------------------------------------------------------------------------

/**
 * The result of comparing transition of a Transition to the time interval
 * of the corresponding AtcMatchingEra.
 */
enum {
  kAtcMatchStatusFarPast, // 0
  kAtcMatchStatusPrior, // 1
  kAtcMatchStatusExactMatch, // 2
  kAtcMatchStatusWithinMatch, // 3
  kAtcMatchStatusFarFuture, // 4
};

/** A struct that represents a matching ZoneEra. */
struct AtcMatchingEra {
  /**
   * The effective start time of the matching ZoneEra, which uses the
   * UTC offsets of the previous matching era.
   */
  struct AtcDateTuple start_dt;

  /** The effective until time of the matching ZoneEra. */
  struct AtcDateTuple until_dt;

  /** The ZoneEra that matched the given year. NonNullable. */
  struct AtcZoneEra *era;

  /** The previous MatchingEra, needed to interpret start_dt.  */
  struct AtcMatchingEra *prev_match;

  /** The STD offset of the last Transition in this MatchingEra. */
  uint16_t last_offset_minutes;

  /** The DST offset of the last Transition in this MatchingEra. */
  uint16_t last_delta_minutes;
};

struct AtcTransition {
  /** The matching_era which generated this Transition. */
  const struct AtcMatchingEra *match;

  /**
   * The Zone transition rule that matched for the the given year. Set to
   * nullptr if the RULES column is '-', indicating that the MatchingEra was
   * a "simple" ZoneEra.
   */
  const struct AtcZoneRule *rule;

  /**
   * The original transition time, usually 'w' but sometimes 's' or 'u'. After
   * expandDateTuple() is called, this field will definitely be a 'w'. We must
   * remember that the transition_time* fields are expressed using the UTC
   * offset of the *previous* Transition.
   */
  struct AtcDateTuple transition_time;

  union {
    /**
     * Version of transition_time in 's' mode, using the UTC offset of the
     * *previous* Transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple transition_time_s;

    /**
     * Start time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple start_dt;
  };

  union {
    /**
     * Version of transition_time in 'u' mode, using the UTC offset of the
     * *previous* transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple transition_time_u;

    /**
     * Until time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple until_dt;
  };

  /** The calculated transition time of the given rule. */
  atc_time_t start_epoch_seconds;

  /**
   * The base offset minutes, not the total effective UTC offset. Note that
   * this is different than basic::Transition::offsetMinutes used by
   * BasicZoneProcessor which is the total effective offsetMinutes. (It may be
   * possible to make this into an effective offsetMinutes (i.e. offsetMinutes
   * + deltaMinutes) but it does not seem worth making that change right now.)
   */
  int16_t offset_minutes;

  /** The DST delta minutes. */
  int16_t delta_minutes;

  /** The calculated effective time zone abbreviation, e.g. "PST" or "PDT". */
  char abbrev[kAtcAbbrevSize];

  /** Storage for the single letter 'letter' field if 'rule' is not null. */
  char letter_buf[2];

  union {
    /**
     * During findCandidateTransitions(), this flag indicates whether the
     * current transition is a valid "prior" transition that occurs before other
     * transitions. It is set by setFreeAgentAsPriorIfValid() if the transition
     * is a prior transition.
     */
    uint8_t is_valid_prior;

    /**
     * During processTransitionMatchStatus(), this flag indicates how the
     * transition falls within the time interval of the MatchingEra.
     */
    uint8_t match_status;
  };
};

struct AtcTransitionResult {
  const struct AtcTransition *transition0; // fold==0
  const struct AtcTransition *transition1; // fold==1
  int8_t search_status;
};

/** The list of transitions for a given time zone. */
struct AtcTransitionStorage {
  struct AtcTransition transition_pool[kAtcTransitionStorageSize];
  struct AtcTransition *transitions[kAtcTransitionStorageSize];
  uint8_t index_prior;
  uint8_t index_candidate;
  uint8_t index_free;

  /** Number of allocated transitions. */
  uint8_t alloc_size;
};

/**
 * Like compare_transition_to_match() except perform a fuzzy match within at
 * least one-month of the match.start or match.until.
 *
 * Return:
 *    * kAtcMatchStatusPrior if t less than match by at least one month
 *    * kAtcMatchStatusWithinMatch if t within match,
 *    * kAtcMatchStatusFarFuture if t greater than match by at least one month
 *    * kAtcMatchStatusExactMatch is never returned, we cannot know that t ==
 *      match.start
 *
 * Exported for testing.
 */
uint8_t atc_processing_compare_transition_to_match_fuzzy(
    const struct AtcTransition *t, const struct AtcMatchingEra *match);

/**
 * Return the most recent year from the Rule[fromYear, toYear] which is
 * prior to the matching ZoneEra years of [startYear, endYear].
 *
 * Return LocalDate::kInvalidYearTiny (-128) if the rule[fromYear,
 * to_year] has no prior year to the MatchingEra[startYear, endYear].
 *
 * Exported for testing.
 *
 * @param fromYear FROM year field of a Rule entry
 * @param toYear TO year field of a Rule entry
 * @param startYear start year of the matching ZoneEra
 * @param endYear until year of the matching ZoneEra (unused)
 */
int8_t atc_processing_get_most_recent_prior_year(
    int8_t from_year, int8_t to_year,
    int8_t start_year, int8_t end_year);

//---------------------------------------------------------------------------
// Externally exported API for converting between epoch seconds and
// LocalDateTime and OffsetDateTime.
//---------------------------------------------------------------------------

/**
 * Zone processing work space. One of these should be created for each active
 * timezone. It can be reused among multiple timezones but a change of timezone
 * causes the internal cache to be wiped and recreated.
 */
struct AtcZoneProcessing {
  const struct AtcZoneInfo *zone_info;
  int16_t year; // maybe create LocalDate::kInvalidYear?
  uint8_t is_filled;
  uint8_t num_matches; // actual number of matches
  struct AtcMatchingEra matches[kAtcMaxMatches];
  struct AtcTransitionStorage transition_storage;
};

// External API
struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  int16_t offset_minutes;
};

// internal, exposed for testing
bool atc_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year);

// internal, exposed for testing
void atc_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds);

// External API
void atc_processing_init(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info);

/** Convert epoch_seconds to odt using the given zone_info. */
void atc_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds,
  const struct AtcZoneInfo *zone_info,
  struct AtcOffsetDateTime *odt);

/** Convert the ldt to the epoch seconds. */
atc_time_t atc_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcLocalDateTime *ldt);

#endif
