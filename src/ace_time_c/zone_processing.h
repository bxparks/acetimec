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
#include "offset_date_time.h" // AtcOffsetDateTime
#include "zone_info.h"
#include "transition.h" // AtcTransition, AtcTransitionStorage

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

/** A tuple of (year_tiny, month). */
struct AtcYearMonth {
  int8_t year_tiny;
  uint8_t month;
};

struct AtcTransitionResult {
  const struct AtcTransition *transition0; // fold==0
  const struct AtcTransition *transition1; // fold==1
  int8_t search_status; // 0-gap, 1=exact, 2=overlap
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
  const struct AtcZoneInfo *zone_info;
  int16_t year; // maybe create LocalDate::kInvalidYear?
  uint8_t is_filled;
  uint8_t num_matches; // actual number of matches
  struct AtcMatchingEra matches[kAtcMaxMatches];
  struct AtcTransitionStorage transition_storage;
};

//---------------------------------------------------------------------------

/** Initialize AtcZoneProcessing for the given zone_info and year. */
bool atc_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  int16_t year);

/**
 * Initialize AtcZoneProcessing for the given zone_info and epoch seconds.
 * Return true upon sucess.
 */
bool atc_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds);

/** Initialize AtcZoneProcessing. Should be called once in the app. */
void atc_processing_init(struct AtcZoneProcessing *processing);

/**
 * Convert epoch_seconds to odt using the given zone_info. Return true upon
 * success.
 */
bool atc_processing_offset_date_time_from_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *odt);

/**
 * Convert the LocalDateTime to OffsetDateTime using the given zone_info.
 * Return true upon success.
 */
bool atc_processing_offset_date_time_from_local_date_time(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  const struct AtcLocalDateTime *ldt,
  struct AtcOffsetDateTime *odt);

#endif
