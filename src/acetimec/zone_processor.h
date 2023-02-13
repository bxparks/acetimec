/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zone_processor.h
 *
 * Data structures and functions related to determining the UTC and DST
 * offsets using the list of DST transitions encoded by the AtcTransition
 * objects.
 */

#ifndef ACE_TIME_C_ZONE_PROCESSOR_H
#define ACE_TIME_C_ZONE_PROCESSOR_H

#include <stdint.h>
#include "common.h" // atc_time_t
#include "../zoneinfo/zone_info.h"
#include "local_date_time.h" // AtcLocalDateTime
#include "date_tuple.h" // AtcDateTuple
#include "transition.h" // AtcTransition, AtcTransitionStorage

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Conversion and accessor utilities.
//---------------------------------------------------------------------------

/** A tuple of month and day. */
typedef struct AtcMonthDay {
  /** month [1,12] */
  uint8_t month;

  /** day [1,31] */
  uint8_t day;
} AtcMonthDay;

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
AtcMonthDay atc_processor_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month);

//---------------------------------------------------------------------------

/**
 * Return the most recent year from the Rule[from_year, to_year] which is
 * prior to the matching ZoneEra years of [start_year, end_year].
 *
 * Return LocalDate::kInvalidYear if the rule[from_year, to_year] has no prior
 * year to the MatchingEra[start_year, end_year].
 *
 * Exported for testing.
 *
 * @param from_year FROM year field of a Rule entry
 * @param to_year TO year field of a Rule entry
 * @param start_year start year of the matching ZoneEra
 * @param end_year until year of the matching ZoneEra (unused)
 */
int16_t atc_processor_get_most_recent_prior_year(
    int16_t from_year, int16_t to_year,
    int16_t start_year, int16_t end_year);

/**
 * Calculate interior years. Up to maxInteriorYears, usually 3 or 4.
 * Returns the number of interior years.
 *
 * Exported for testing.
 */
uint8_t atc_processor_calc_interior_years(
    int16_t* interior_years,
    uint8_t max_interior_years,
    int16_t from_year,
    int16_t to_year,
    int16_t start_year,
    int16_t end_year);

//---------------------------------------------------------------------------
// Data structures related to the AtcZoneProcessor object which is responsible
// for finding the active Transitions of a time zone, and for finding the
// matching Transitions at a gien epoch_seconds or LocalDatetime.
//---------------------------------------------------------------------------

/**
 * Zone processor work space. One of these should be created for each active
 * timezone. It can be reused among multiple timezones but a change of timezone
 * causes the internal cache to be wiped and recreated.
 */
typedef struct AtcZoneProcessor {
  /** The time zone attached to this Processor workspace. */
  const AtcZoneInfo *zone_info;

  /** Cache year [0,9999] */
  int16_t year;

  /** True if the cache is valid. */
  uint8_t is_filled;

  /** Number of valid matches in the array. */
  uint8_t num_matches;

  /** The matching eras for the current zone and year. */
  AtcMatchingEra matches[kAtcMaxMatches];

  /** Pool of transitions relevant for the current zone and year */
  AtcTransitionStorage transition_storage;
} AtcZoneProcessor;

/** Values of the the AtcFindResult.type field. */
enum {
  kAtcFindResultNotFound = 0,
  kAtcFindResultExact = 1,
  kAtcFindResultGap = 2,
  kAtcFindResultOverlap = 3,
};

/**
 * Data structure that converts the AtcTransitionForSeconds and
 * AtcTransitionForDatetime into time offsets and other extra information which
 * can be used to construct an AtcOffsetDateTime or an AtcZonedExtra.
 *
 * Adapted from FindResult in ZoneProcessor.h of the AceTime library.
 */
typedef struct AtcFindResult {
  /**
   * The result type of the find_by_xxx() function, one of the
   * kAtcFindResultXxx enums.
   */
  uint8_t type;

  /** The fold of the resulting OffsetDateTime. */
  uint8_t fold;

  /** The STD offset of the target OffsetDateTime. */
  int16_t std_offset_minutes;

  /** The DST offset of the target OffsetDateTime. */
  int16_t dst_offset_minutes;

  /**
   * The STD offset of the requested LocalDateTime when
   * atc_processor_find_by_local_date_time() finds a gap. Otherwise, this will
   * be identical to std_offset_minutes.
   */
  int16_t req_std_offset_minutes;

  /**
   * The DST offset of the requested LocalDateTime when
   * atc_processor_find_by_local_date_time() finds a gap. Otherwise, this will
   * be identical to dst_offset_minutes.
   */
  int16_t req_dst_offset_minutes;

  /**
   * Contains a pointer to a transition string buffer. The string should be
   * copied by the calling code as soon as possible.
   */
  const char *abbrev;
} AtcFindResult;

//---------------------------------------------------------------------------
// Externally exported API for converting between epoch seconds and
// LocalDateTime and OffsetDateTime.
//---------------------------------------------------------------------------

/**
 * Initialize AtcZoneProcessor data structure. This needs to be called only
 * once for each instance of AtcZoneProcessor.
 */
void atc_processor_init(AtcZoneProcessor *processor);

/**
 * Initialize AtcZoneProcessor for the given zone_info and year.
 * Return non-zero error code upon failure.
 */
int8_t atc_processor_init_for_year(
  AtcZoneProcessor *processor,
  const AtcZoneInfo *zone_info,
  int16_t year);

/**
 * Initialize AtcZoneProcessor for the given zone_info and epoch seconds.
 * Return non-zero error code upon failure.
 */
int8_t atc_processor_init_for_epoch_seconds(
  AtcZoneProcessor *processor,
  const AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds);

/** Find the AtcFindResult at the given epoch_seconds. */
int8_t atc_processor_find_by_epoch_seconds(
    AtcZoneProcessor *processor,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcFindResult *result);

/**
 * Find the AtcFindResult at the given LocalDateTime and fold. The fold
 * parameter is used only when LocalDateTime falls in a gap or an overlap.
 * library. Return non-zero error code upon failure.
 */
int8_t atc_processor_find_by_local_date_time(
    AtcZoneProcessor *processor,
    const AtcZoneInfo *zone_info,
    const AtcLocalDateTime *ldt,
    AtcFindResult *result);

//---------------------------------------------------------------------------
// Functions and data structures related to the creation of the active
// Transitions of the given time zone at the given year.
// Most of these are internal function which are exposed for testing.
//---------------------------------------------------------------------------

/** A tuple of (year, month). */
typedef struct AtcYearMonth {
  /** year [0,10000] */
  int16_t year;
  /** month [1,12] */
  uint8_t month;
} AtcYearMonth;

/** Find ZoneEra entries which match the [start_ym, until_ym) interval. */
uint8_t atc_processor_find_matches(
  const AtcZoneInfo *zone_info,
  AtcYearMonth start_ym,
  AtcYearMonth until_ym,
  AtcMatchingEra *matches,
  uint8_t num_matches);

/**
 * Determine if era is less than (-1), roughly equal (0), or greater than (1)
 * the given  (year, month).
 */
int8_t atc_compare_era_to_year_month(
    const AtcZoneEra *era,
    int16_t year,
    uint8_t month);

/** Create new new_match, containing the given parameters. */
void atc_create_matching_era(
    AtcMatchingEra *new_match,
    AtcMatchingEra *prev_match,
    const AtcZoneEra *era,
    AtcYearMonth start_ym,
    AtcYearMonth until_ym);

/** Populate the AtcDateTuple dt with the given year and rule. */
void atc_processor_get_transition_time(
    int16_t year,
    const AtcZoneRule* rule,
    AtcDateTuple *dt);

/** Create an AtcTransition t for the given rule and match. */
void atc_processor_create_transition_for_year(
    AtcTransition *t,
    int16_t year,
    const AtcZoneRule *rule,
    const AtcMatchingEra *match,
    const char * const *letters /*nullable*/);

/**
 * Evaluate the given 'match' and add candidate AtcTransitions into the
 * AtcTransitionStorage ts.
 */
void atc_processor_find_candidate_transitions(
    AtcTransitionStorage *ts,
    AtcMatchingEra *match);

/** Calculate the given transition's `match_status`. */
void atc_processor_process_transition_match_status(
    AtcTransition *transition,
    AtcTransition **prior);

/** Create transitions for all the MatchingEras */
void atc_processor_create_transitions(
  AtcTransitionStorage *ts,
  AtcMatchingEra *matches,
  uint8_t num_matches);

/** Create active transitions for the given match. */
void atc_processor_create_transitions_for_match(
  AtcTransitionStorage *ts,
  AtcMatchingEra *match);

/** Create active transitions for the given simple match (no rules). */
void atc_processor_create_transitions_from_simple_match(
    AtcTransitionStorage *ts,
    AtcMatchingEra *match);

/** Create active transitions for the given named match (with rules). */
void atc_processor_create_transitions_from_named_match(
    AtcTransitionStorage *ts,
    AtcMatchingEra *match);

/** Update the start and until times of the specified transitions. */
void atc_processor_generate_start_until_times(
    AtcTransition **begin,
    AtcTransition **end);

/** Compute the time zone abbreviation of the specified transitions. */
void atc_processor_calc_abbreviations(
    AtcTransition **begin,
    AtcTransition **end);

/** Compute the time zone abbreviation for the given parameters. */
void atc_processor_create_abbreviation(
    char *dest,
    uint8_t dest_size,
    const char *format,
    int16_t delta_minutes,
    const char *letter_string);

//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
