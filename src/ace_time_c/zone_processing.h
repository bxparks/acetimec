/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_PROCESSING_H
#define ACE_TIME_C_ZONE_PROCESSING_H

#include <stdint.h>
#include "common.h" // atc_time_t
#include "local_date_time.h" // AtcLocalDateTime
#include "offset_date_time.h" // AtcOffsetDateTime
#include "zone_info.h"
#include "transition.h" // AtcTransition, AtcTransitionStorage

//---------------------------------------------------------------------------
// Conversion and accessor utilities.
//---------------------------------------------------------------------------

/** A tuple of month and day. */
struct AtcMonthDay {
  /** month [1,12] */
  uint8_t month;

  /** day [1,31] */
  uint8_t day;
};

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
struct AtcMonthDay atc_processing_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month);

//---------------------------------------------------------------------------
// Data structures to track ZoneEra transitions and associated info.
//---------------------------------------------------------------------------

enum {
  kAtcSearchStatusGap = 0,
  kAtcSearchStatusExact = 1,
  kAtcSearchStatusOverlap = 2,
};

/**
 * The transition search result at a particular epoch second or local date
 * time.
 */
struct AtcTransitionResult {
  /** Transition for fold==0 */
  const struct AtcTransition *transition0;

  /** Transition for fold==1 */
  const struct AtcTransition *transition1;

  /** Result of search: 0=gap, 1=exact, 2=overlap */
  int8_t search_status;
};

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

/**
 * Calculate interior years. Up to maxInteriorYears, usually 3 or 4.
 * Returns the number of interior years.
 *
 * Exported for testing.
 */
uint8_t atc_processing_calc_interior_years(
    int8_t* interior_years,
    uint8_t max_interior_years,
    int8_t from_year,
    int8_t to_year,
    int8_t start_year,
    int8_t end_year);

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
  /** The time zone attached to this Processing workspace. */
  const struct AtcZoneInfo *zone_info;

  /** Cache year [0,9999] */
  int16_t year;

  /** True if the cache is valid. */
  uint8_t is_filled;

  /** Number of valid matches in the array. */
  uint8_t num_matches;

  /** The matching eras for the current zone and year. */
  struct AtcMatchingEra matches[kAtcMaxMatches];

  /** Pool of transitions relevant for the current zone and year */
  struct AtcTransitionStorage transition_storage;
};

//---------------------------------------------------------------------------

/**
 * Initialize AtcZoneProcessing data structure. This needs to be called only
 * once for each instance of AtcZoneProcessing.
 */
void atc_processing_init(struct AtcZoneProcessing *processing);

/**
 * Initialize AtcZoneProcessing for the given zone_info and year.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  int16_t year);

/**
 * Initialize AtcZoneProcessing for the given zone_info and epoch seconds.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds);

/**
 * Convert epoch_seconds to an AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_offset_date_time_from_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *odt);

/**
 * Convert the LocalDateTime to OffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_offset_date_time_from_local_date_time(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  const struct AtcLocalDateTime *ldt,
  uint8_t fold,
  struct AtcOffsetDateTime *odt);

//---------------------------------------------------------------------------

/**
 * Additional meta information about the transition. Should be identical to
 * AtcZoneExtra.
 */
struct AtcTransitionInfo {
  /** STD offset */
  int16_t std_offset_minutes;
  /** DST offset */
  int16_t dst_offset_minutes;
  /** abbreviation (e.g. PST, PDT) */
  char abbrev[kAtcAbbrevSize];
};

/**
 * Find the AtcTransitionInfo (i.e. STD offset, DST offset, abbrev)
 * at the given epoch_seconds.
 * Return non-zero error code upon failure.
 */
int8_t atc_processing_transition_info_from_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcTransitionInfo *ti);

//---------------------------------------------------------------------------
// Functions and data structures exposed for testing.
//---------------------------------------------------------------------------

/** A tuple of (year_tiny, month). */
struct AtcYearMonth {
  /** (year-kEpochYear) [-126, 126] */
  int8_t year_tiny;
  /** month [1,12] */
  uint8_t month;
};

uint8_t atc_processing_find_matches(
  const struct AtcZoneInfo *zone_info,
  struct AtcYearMonth start_ym,
  struct AtcYearMonth until_ym,
  struct AtcMatchingEra *matches,
  uint8_t num_matches);

int8_t atc_compare_era_to_year_month(
    const struct AtcZoneEra *era,
    int8_t year_tiny,
    uint8_t month);

void atc_create_matching_era(
    struct AtcMatchingEra *new_match,
    struct AtcMatchingEra *prev_match,
    const struct AtcZoneEra *era,
    struct AtcYearMonth start_ym,
    struct AtcYearMonth until_ym);

void atc_processing_get_transition_time(
    int8_t year_tiny,
    const struct AtcZoneRule* rule,
    struct AtcDateTuple *dt);

void atc_processing_create_transition_for_year(
    struct AtcTransition *t,
    int8_t year_tiny,
    const struct AtcZoneRule *rule,
    const struct AtcMatchingEra *match);

void atc_processing_find_candidate_transitions(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match);

void atc_processing_process_transition_match_status(
    struct AtcTransition *transition,
    struct AtcTransition **prior);

void atc_processing_create_transitions_from_named_match(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match);

void atc_processing_generate_start_until_times(
    struct AtcTransition **begin,
    struct AtcTransition **end);

void atc_processing_create_abbreviation(
    char* dest,
    uint8_t dest_size,
    const char* format,
    uint16_t delta_minutes,
    const char* letter_string);

#endif
