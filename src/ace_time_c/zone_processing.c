/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdbool.h>
#include "zone_processing.h"

//---------------------------------------------------------------------------
// Internal helper methods.
//---------------------------------------------------------------------------

uint16_t atc_zone_info_time_code_to_minutes(
  uint8_t code, uint8_t modifier)
{
  return code * (uint16_t) 15 + (modifier & 0x0f);
}

uint8_t atc_zone_info_modifier_to_suffix(uint8_t modifier)
{
  return modifier & 0xf0;
}

int8_t atc_compare_internal_date_time(
  const struct AtcDateTime *a,
  const struct AtcDateTime *b)
{
  if (a->year_tiny < b->year_tiny) return -1;
  if (a->year_tiny > b->year_tiny) return 1;
  if (a->month < b->month) return -1;
  if (a->month > b->month) return 1;
  if (a->day < b->day) return -1;
  if (a->day > b->day) return 1;
  if (a->minutes < b->minutes) return -1;
  if (a->minutes > b->minutes) return 1;
  return 0;
}

void atc_processing_transition_storage_init(
  struct AtcTransitionStorage *transition_storage)
{
  for (int i = 0; i < kAtcTransitionStorageSize; i++) {
    transition_storage->transitions[i] =
        &transition_storage->transition_pool[i];
  }
  transition_storage->index_prior = 0;
  transition_storage->index_candidate = 0;
  transition_storage->index_free = 0;
}

/** Return (1, 0, -1) depending on how era compares to (year_tiny, month). */
int8_t atc_compare_era_to_year_month(
    const struct AtcZoneEra *era,
    int8_t year_tiny,
    uint8_t month)
{
  if (era->until_year_tiny < year_tiny) return -1;
  if (era->until_year_tiny > year_tiny) return 1;
  if (era->until_month < month) return -1;
  if (era->until_month > month) return 1;
  if (era->until_day > 1) return 1;
  //if (era->until_time_minutes < 0) return -1; // never possible
  if (era->until_time_code > 0) return 1;
  return 0;
}

/**
 * Determines if era overlaps the interval [startYm, untilYm). This does
 * not need to be exact since the startYm and untilYm are created to have
 * some slop of about one month at the low and high end, so we can ignore
 * the day, time and timeSuffix fields of the era. The start date of the
 * current era is represented by the UNTIL fields of the previous era, so
 * the interval of the current era is [era.start=prev.UNTIL,
 * era.until=era.UNTIL). Overlap happens if (era.start < untilYm) and
 * (era.until > startYm). If prev.isNull(), then interpret prev as the
 * earliest ZoneEra.
 */
bool atc_era_overlaps_interval(
  const struct AtcMatchingEra *prev_match,
  const struct AtcZoneEra *era,
  struct AtcYearMonth start_ym,
  struct AtcYearMonth until_ym)
{
  return (prev_match == NULL || atc_compare_era_to_year_month(
          prev_match->era, until_ym.year_tiny, until_ym.month) < 0)
      && atc_compare_era_to_year_month(
          era, start_ym.year_tiny, start_ym.month) > 0;
}

/**
 * Create a new MatchingEra object around the 'era' which intersects the
 * half-open [startYm, untilYm) interval. The interval is assumed to overlap
 * the ZoneEra using the eraOverlapsInterval() method. The 'prev' ZoneEra is
 * needed to define the startDateTime of the current era.
 */
void atc_create_matching_era(
    struct AtcMatchingEra *new_match,
    struct AtcMatchingEra *prev_match,
    struct AtcZoneEra *era,
    struct AtcYearMonth start_ym,
    struct AtcYearMonth until_ym) {

  // If prev_match is null, set start_date to be earlier than all valid
  // ZoneEra.
  struct AtcDateTime start_date;
  if (prev_match == NULL) {
    start_date.year_tiny = kAtcInvalidYearTiny;
    start_date.month = 1;
    start_date.day = 1;
    start_date.minutes = 0;
    start_date.suffix = kAtcSuffixW;
  } else {
    start_date.year_tiny = prev_match->era->until_year_tiny;
    start_date.month = prev_match->era->until_month;
    start_date.day = prev_match->era->until_day;
    start_date.minutes = atc_zone_info_time_code_to_minutes(
        prev_match->era->until_time_code,
        prev_match->era->until_time_modifier);
    start_date.suffix = atc_zone_info_modifier_to_suffix(
        prev_match->era->until_time_modifier);
  }
  struct AtcDateTime lower_bound = {
    start_ym.year_tiny,
    start_ym.month,
    1,
    0,
    kAtcSuffixW
  };
  if (atc_compare_internal_date_time(&start_date, &lower_bound) < 0) {
    start_date = lower_bound;
  }

  struct AtcDateTime until_date = {
    era->until_year_tiny,
    era->until_month,
    era->until_day,
    atc_zone_info_time_code_to_minutes(
        era->until_time_code,
        era->until_time_modifier),
    atc_zone_info_modifier_to_suffix(era->until_time_modifier),
  };
  struct AtcDateTime upper_bound = {
    until_ym.year_tiny,
    until_ym.month,
    1,
    0,
    kAtcSuffixW
  };
  if (atc_compare_internal_date_time(&upper_bound, &until_date) < 0) {
    until_date = upper_bound;
  }

  new_match->start_dt = start_date;
  new_match->until_dt = until_date;
  new_match->era = era;
  new_match->prev_match = prev_match;
  new_match->last_offset_minutes = 0;
  new_match->last_delta_minutes = 0;
}

int atc_zone_processing_find_matches(
  const struct AtcZoneInfo *zone_info,
  struct AtcYearMonth start_ym,
  struct AtcYearMonth until_ym,
  struct AtcMatchingEra *matches,
  uint8_t num_matches)
{
  uint8_t i_match = 0;
  struct AtcMatchingEra *prev_match = NULL;
  for (uint8_t i_era = 0; i_era < zone_info->num_eras; i_era++) {
    struct AtcZoneEra *eras = (struct AtcZoneEra*) zone_info->eras;
    struct AtcZoneEra *era = &eras[i_era];
    if (atc_era_overlaps_interval(prev_match, era, start_ym, until_ym)) {
      if (i_match < num_matches) {
        atc_create_matching_era(
            &matches[i_match], prev_match, era, start_ym, until_ym);
        prev_match = &matches[i_match];
        i_match++;
      }
    }
  }
  return i_match;
}

//---------------------------------------------------------------------------

void atc_zone_processing_init(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info)
{
  processing->zone_info = NULL;
  processing->is_filled = 0;
  processing->num_matches = 0;
  processing->zone_info = zone_info;
}

bool atc_zone_processing_is_filled_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  return processing->is_filled && (year == processing->year);
}

bool atc_zone_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  if (atc_zone_processing_is_filled_for_year(processing, year)) return true;

  processing->year = year;
  processing->num_matches = 0;
  atc_processing_transition_storage_init(&processing->transition_storage);
  const struct AtcZoneContext *context = processing->zone_info->zone_context;
  if (year < context->start_year - 1 || context->until_year < year) {
    return false;
  }
  struct AtcYearMonth start_ym = { (int8_t) (year - kAtcEpochYear - 1), 12 };
  struct AtcYearMonth until_ym = { (int8_t) (year - kAtcEpochYear + 1), 2 };

  // Step 1: Find matches.
  int num_matches = atc_zone_processing_find_matches(
    processing->zone_info,
    start_ym,
    until_ym,
    processing->matches,
    kAtcMaxMatches);
  (void) num_matches;

  // Step 2: Create Transitions.
  // Step 3: Fix transition times.
  // Step 4: Generate start and until times.
  // Step 5: Calc abbreviations.

  return true;
}

void atc_zone_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds)
{
  (void) processing;
  (void) zone_info;
  (void) epoch_seconds;
}

void atc_zone_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *offset_dt)
{
  (void) processing;
  (void) zone_info;
  (void) epoch_seconds;
  (void) offset_dt;
}

atc_time_t atc_zone_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  struct AtcLocalDateTime *local_dt)
{
  (void) processing;
  (void) zone_info;
  (void) local_dt;
  return 0;
}
