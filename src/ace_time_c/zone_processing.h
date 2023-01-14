/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zone_processing.h
 *
 * Data structures and functions related to determining the UTC and DST
 * offsets using the list of DST transitions encoded by the AtcTransition
 * objects.
 */

#ifndef ACE_TIME_C_ZONE_PROCESSING_H
#define ACE_TIME_C_ZONE_PROCESSING_H

#include <stdint.h>
#include "common.h" // atc_time_t
#include "local_date_time.h" // AtcLocalDateTime
#include "offset_date_time.h" // AtcOffsetDateTime
#include "zone_info.h"
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
AtcMonthDay atc_processing_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month);

//---------------------------------------------------------------------------
// Data structures that hold the matching transition when searching by
// AtcLocalDateTime or epoch_seconds.
//---------------------------------------------------------------------------

/**
 * The result returned by atc_processing_find_transition_for_date_time() when
 * searching for transitions by local date time. There are 5 possibilities:
 *
 *  * num=0, prev==NULL, curr=curr: datetime is far past
 *  * num=1, prev==prev, curr=prev: exact match to datetime
 *  * num=2, prev==prev, curr=curr: datetime in overlap
 *  * num=0, prev==prev, curr=curr: datetime in gap
 *  * num=0, prev==prev, curr=NULL: datetime is far future
 *
 * Adapted from TransitionForDateTime in Transition.h of the AceTime library.
 *
 */
typedef struct AtcTransitionForDateTime {
  /** The previous transition, or null if the first transition matches. */
  const AtcTransition *prev;

  /** The matching transition or null if not found. */
  const AtcTransition *curr;

  /** Number of matches for given LocalDateTime: 0, 1, or 2. */
  uint8_t num;
} AtcTransitionForDateTime;

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
 * The 'abbrev' field contains a pointer to a transition string buffer. The
 * string should be copied by the calling code as soon as possible.
 *
 * Adapted from FindResult in ZoneProcessor.h of the AceTime library.
 */
typedef struct AtcFindResult {
  uint8_t type;
  uint8_t fold;
  int16_t std_offset_minutes;
  int16_t dst_offset_minutes;
  int16_t req_std_offset_minutes;
  int16_t req_dst_offset_minutes;
  const char *abbrev;
} AtcFindResult;

//---------------------------------------------------------------------------

/**
 * Return the most recent year from the Rule[fromYear, toYear] which is
 * prior to the matching ZoneEra years of [startYear, endYear].
 *
 * Return LocalDate::kInvalidYear if the rule[fromYear, to_year] has no prior
 * year to the MatchingEra[startYear, endYear].
 *
 * Exported for testing.
 *
 * @param fromYear FROM year field of a Rule entry
 * @param toYear TO year field of a Rule entry
 * @param startYear start year of the matching ZoneEra
 * @param endYear until year of the matching ZoneEra (unused)
 */
int16_t atc_processing_get_most_recent_prior_year(
    int16_t from_year, int16_t to_year,
    int16_t start_year, int16_t end_year);

/**
 * Calculate interior years. Up to maxInteriorYears, usually 3 or 4.
 * Returns the number of interior years.
 *
 * Exported for testing.
 */
uint8_t atc_processing_calc_interior_years(
    int16_t* interior_years,
    uint8_t max_interior_years,
    int16_t from_year,
    int16_t to_year,
    int16_t start_year,
    int16_t end_year);

//---------------------------------------------------------------------------
// Externally exported API for converting between epoch seconds and
// LocalDateTime and OffsetDateTime.
//---------------------------------------------------------------------------

/**
 * Zone processing work space. One of these should be created for each active
 * timezone. It can be reused among multiple timezones but a change of timezone
 * causes the internal cache to be wiped and recreated.
 */
typedef struct AtcZoneProcessing {
  /** The time zone attached to this Processing workspace. */
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
} AtcZoneProcessing;

//---------------------------------------------------------------------------

/**
 * Initialize AtcZoneProcessing data structure. This needs to be called only
 * once for each instance of AtcZoneProcessing.
 */
void atc_processing_init(AtcZoneProcessing *processing);

/**
 * Initialize AtcZoneProcessing for the given zone_info and year.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_init_for_year(
  AtcZoneProcessing *processing,
  const AtcZoneInfo *zone_info,
  int16_t year);

/**
 * Initialize AtcZoneProcessing for the given zone_info and epoch seconds.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_init_for_epoch_seconds(
  AtcZoneProcessing *processing,
  const AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds);

/** Find the AtcFindResult at the given epoch_seconds. */
int8_t atc_processing_find_by_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcFindResult *result);

/**
 * Convert epoch_seconds to an AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_offset_date_time_from_epoch_seconds(
  AtcZoneProcessing *processing,
  const AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  AtcOffsetDateTime *odt);

/**
 * Convert the LocalDateTime to OffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_offset_date_time_from_local_date_time(
  AtcZoneProcessing *processing,
  const AtcZoneInfo *zone_info,
  const AtcLocalDateTime *ldt,
  uint8_t fold,
  AtcOffsetDateTime *odt);

//---------------------------------------------------------------------------
// Functions and data structures exposed for testing.
//---------------------------------------------------------------------------

/** A tuple of (year, month). */
typedef struct AtcYearMonth {
  /** year [0,10000] */
  int16_t year;
  /** month [1,12] */
  uint8_t month;
} AtcYearMonth;

uint8_t atc_processing_find_matches(
  const AtcZoneInfo *zone_info,
  AtcYearMonth start_ym,
  AtcYearMonth until_ym,
  AtcMatchingEra *matches,
  uint8_t num_matches);

int8_t atc_compare_era_to_year_month(
    const AtcZoneEra *era,
    int16_t year,
    uint8_t month);

void atc_create_matching_era(
    AtcMatchingEra *new_match,
    AtcMatchingEra *prev_match,
    const AtcZoneEra *era,
    AtcYearMonth start_ym,
    AtcYearMonth until_ym);

void atc_processing_get_transition_time(
    int16_t year,
    const AtcZoneRule* rule,
    AtcDateTuple *dt);

void atc_processing_create_transition_for_year(
    AtcTransition *t,
    int16_t year,
    const AtcZoneRule *rule,
    const AtcMatchingEra *match);

void atc_processing_find_candidate_transitions(
    AtcTransitionStorage *ts,
    AtcMatchingEra *match);

void atc_processing_process_transition_match_status(
    AtcTransition *transition,
    AtcTransition **prior);

void atc_processing_create_transitions_from_named_match(
    AtcTransitionStorage *ts,
    AtcMatchingEra *match);

void atc_processing_generate_start_until_times(
    AtcTransition **begin,
    AtcTransition **end);

void atc_processing_create_abbreviation(
    char* dest,
    uint8_t dest_size,
    const char* format,
    int16_t delta_minutes,
    const char* letter_string);

//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
