/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdbool.h>
#include "local_date.h" // atc_days_in_year_month()
#include "zone_processing.h"

//---------------------------------------------------------------------------
// ZoneInfo, ZoneEra, ZoneRule, ZonePolicy helpers
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

//---------------------------------------------------------------------------

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

void atc_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month,
    uint8_t *result_month,
    uint8_t *result_day)
{
  if (on_day_of_week == 0) {
    *result_month = month;
    *result_day = on_day_of_month;
    return;
  }

  if (on_day_of_month >= 0) {
    uint8_t days_in_month = atc_days_in_year_month(year, month);
    if (on_day_of_month == 0) {
      on_day_of_month = days_in_month - 6;
    }
    uint8_t dow = atc_day_of_week(year, month, on_day_of_month);
    uint8_t day_of_week_shift = (on_day_of_week - dow + 7) % 7;
    uint8_t day = (uint8_t) (on_day_of_week + day_of_week_shift);
    if (day > days_in_month) {
      // TODO: Support shifting from Dec to Jan of following  year.
      day -= days_in_month;
      month++;
    }
    *result_month = month;
    *result_day = day;
  } else {
    on_day_of_month = -on_day_of_month;
    uint8_t dow = atc_day_of_week(year, month, on_day_of_month);
    int8_t day_of_week_shift = (dow - on_day_of_week + 7) % 7;
    int8_t day = on_day_of_week - day_of_week_shift;
    if (day < 1) {
      // TODO: Support shifting from Jan to Dec of the previous year.
      month--;
      uint8_t days_in_prev_month = atc_days_in_year_month(year, month);
      day += days_in_prev_month;
    }
    *result_month = month;
    *result_day = (uint8_t) day;
  }
}

//---------------------------------------------------------------------------
// Step 1
//---------------------------------------------------------------------------

uint8_t atc_zone_processing_find_matches(
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

// ---------------------------------------------------------------------------
// Step 2
// ---------------------------------------------------------------------------

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

struct AtcTransition *atc_zone_processing_get_free_agent(
    struct AtcTransitionStorage *transition_storage)
{
  if (transition_storage->index_free < kAtcTransitionStorageSize) {
    if (transition_storage->index_free >= transition_storage->alloc_size) {
      transition_storage->alloc_size = transition_storage->index_free + 1;
    }
    return transition_storage->transitions[transition_storage->index_free];
  } else {
    /* No more transition available in the buffer, so just return the last
     * one. This will probably cause a bug in the timezone calculations, but
     * I think this is better than triggering undefined behavior by running
     * off the end of the mTransitions buffer.
     */
    return transition_storage->transitions[kAtcTransitionStorageSize - 1];
  }
}

void atc_zone_processing_get_transition_time(
    int8_t year_tiny,
    const struct AtcZoneRule* rule,
    struct AtcDateTime *dt)
{
  uint8_t month;
  uint8_t day;
  atc_calc_start_day_of_month(
      year_tiny + kAtcEpochYear,
      rule->in_month,
      rule->on_day_of_week,
      rule->on_day_of_month,
      &month,
      &day);

  dt->year_tiny = year_tiny;
  dt->month = month;
  dt->day = day;
  dt->minutes = atc_zone_info_time_code_to_minutes(
      rule->at_time_code,
      rule->at_time_modifier);
  dt->suffix = atc_zone_info_modifier_to_suffix(rule->at_time_modifier);
}

void atc_zone_processing_create_transition_for_year(
    struct AtcTransition *t,
    int8_t year_tiny,
    const struct AtcZoneRule *rule,
    const struct AtcMatchingEra *match)
{
  t->match = match;
  t->rule = rule;
  t->offset_minutes = atc_zone_info_time_code_to_minutes(
      match->era->offset_code, 0);
  t->letter_buf[0] = '\0';

  if (rule) {
    atc_zone_processing_get_transition_time(
        year_tiny, rule, &t->transition_time);
    t->delta_minutes = atc_zone_info_time_code_to_minutes(rule->delta_code, 0);
    char letter = rule->letter;
    if (letter >= 32) {
      // If LETTER is a '-', treat it the same as an empty string.
      if (letter != '-') {
        t->letter_buf[0] = letter;
        t->letter_buf[1] = '\0';
      }
    } else {
      // rule->letter is a long string, so is referenced as an offset index into
      // the ZonePolicy.letters array. The string cannot fit in letter_buf, so
      // will be retrieved by the atc_zone_processing_long_letter() function.
    }
  } else {
    // Create a Transition using the MatchingEra for the transitionTime.
    // Used for simple MatchingEra.
    t->transition_time = match->start_dt;
    t->delta_minutes = atc_zone_info_time_code_to_minutes(
        match->era->delta_code, 0);
  }
}

void atc_zone_processing_add_free_agent_to_active_pool(
    struct AtcTransitionStorage *ts)
{
  if (ts->index_free >= kAtcTransitionStorageSize) return;
  ts->index_free++;
  ts->index_prior = ts->index_free;
  ts->index_candidate = ts->index_free;
}

void atc_zone_processing_create_transitions_from_simple_match(
    struct AtcTransitionStorage *transition_storage,
    struct AtcMatchingEra *match)
{
  struct AtcTransition *free_agent = atc_zone_processing_get_free_agent(
    transition_storage);
  atc_zone_processing_create_transition_for_year(free_agent, 0, NULL, match);
  free_agent->match_status = kAtcMatchStatusExactMatch;
  match->last_offset_minutes = free_agent->offset_minutes;
  match->last_delta_minutes = free_agent->delta_minutes;
  atc_zone_processing_add_free_agent_to_active_pool(
      transition_storage);
}

void atc_zone_processing_create_transitions_from_named_match(
    struct AtcTransitionStorage *transition_storage,
    struct AtcMatchingEra *match)
{
  (void) transition_storage;
  (void) match;
}

void atc_zone_processing_create_transitions_for_match(
  struct AtcTransitionStorage *transition_storage,
  struct AtcMatchingEra *match)
{
  const struct AtcZonePolicy *policy = match->era->zone_policy;
  if (policy == NULL) {
    atc_zone_processing_create_transitions_from_simple_match(
        transition_storage, match);
  } else {
    atc_zone_processing_create_transitions_from_named_match(
        transition_storage, match);
  }
}

void atc_zone_processing_create_transitions(
  struct AtcTransitionStorage *transition_storage,
  struct AtcMatchingEra *matches,
  uint8_t num_matches)
{
  for (uint8_t i = 0; i < num_matches; i++) {
    atc_zone_processing_create_transitions_for_match(
      transition_storage, &matches[i]);
  }
}

//---------------------------------------------------------------------------
// Initialization of AtcZoneProcessing.
//---------------------------------------------------------------------------

void atc_zone_processing_init(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info)
{
  processing->zone_info = zone_info;
  processing->is_filled = 0;
  processing->num_matches = 0;
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
  uint8_t num_matches = atc_zone_processing_find_matches(
    processing->zone_info,
    start_ym,
    until_ym,
    processing->matches,
    kAtcMaxMatches);

  // Step 2: Create Transitions.
  atc_zone_processing_create_transitions(
    &processing->transition_storage,
    processing->matches,
    num_matches);

  (void) num_matches;

  // Step 3: Fix transition times.
  // Step 4: Generate start and until times.
  // Step 5: Calc abbreviations.

  return true;
}

void atc_zone_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds)
{
  (void) processing;
  (void) epoch_seconds;
}

//---------------------------------------------------------------------------
// Public API.
//---------------------------------------------------------------------------

void atc_zone_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *offset_dt)
{
  (void) processing;
  (void) epoch_seconds;
  (void) offset_dt;
}

atc_time_t atc_zone_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcLocalDateTime *local_dt)
{
  (void) processing;
  (void) local_dt;
  return 0;
}
