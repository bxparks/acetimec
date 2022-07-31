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

void atc_processing_calc_start_day_of_month(
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
    uint8_t days_in_month = atc_local_date_days_in_year_month(year, month);
    if (on_day_of_month == 0) {
      on_day_of_month = days_in_month - 6;
    }
    uint8_t dow = atc_local_date_day_of_week(year, month, on_day_of_month);
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
    uint8_t dow = atc_local_date_day_of_week(year, month, on_day_of_month);
    int8_t day_of_week_shift = (dow - on_day_of_week + 7) % 7;
    int8_t day = on_day_of_week - day_of_week_shift;
    if (day < 1) {
      // TODO: Support shifting from Jan to Dec of the previous year.
      month--;
      uint8_t days_in_prev_month = atc_local_date_days_in_year_month(
          year, month);
      day += days_in_prev_month;
    }
    *result_month = month;
    *result_day = (uint8_t) day;
  }
}

//---------------------------------------------------------------------------
// TransitionStorage helpers
//---------------------------------------------------------------------------

void atc_transition_storage_init(struct AtcTransitionStorage *ts)
{
  for (int i = 0; i < kAtcTransitionStorageSize; i++) {
    ts->transitions[i] = &ts->transition_pool[i];
  }
  ts->index_prior = 0;
  ts->index_candidate = 0;
  ts->index_free = 0;
}

/**
 * Return a pointer to the first Transition in the free pool. If this
 * transition is not used, then it's ok to just drop it. The next time
 * getFreeAgent() is called, the same Transition will be returned.
 */
struct AtcTransition *atc_transition_storage_get_free_agent(
    struct AtcTransitionStorage *ts)
{
  if (ts->index_free < kAtcTransitionStorageSize) {
    if (ts->index_free >= ts->alloc_size) {
      ts->alloc_size = ts->index_free + 1;
    }
    return ts->transitions[ts->index_free];
  } else {
    /* No more transition available in the buffer, so just return the last
     * one. This will probably cause a bug in the timezone calculations, but
     * I think this is better than triggering undefined behavior by running
     * off the end of the mTransitions buffer.
     */
    return ts->transitions[kAtcTransitionStorageSize - 1];
  }
}

/**
 * Immediately add the free agent Transition at index mIndexFree to the
 * Active pool. Then increment mIndexFree to consume the free agent
 * from the Free pool. This assumes that the Pending and Candidate pool are
 * empty, which makes the Active pool come immediately before the Free
 * pool.
 */
void atc_transition_storage_add_free_agent_to_active_pool(
    struct AtcTransitionStorage *ts)
{
  if (ts->index_free >= kAtcTransitionStorageSize) return;
  ts->index_free++;
  ts->index_prior = ts->index_free;
  ts->index_candidate = ts->index_free;
}

void atc_transition_storage_reset_candidate_pool(
    struct AtcTransitionStorage *ts)
{
  ts->index_candidate = ts->index_prior;
  ts->index_free = ts->index_prior;
}

struct AtcTransition **atc_transition_storage_reserve_prior(
    struct AtcTransitionStorage *ts)
{
  (void) atc_transition_storage_get_free_agent(ts);
  ts->index_candidate++;
  ts->index_free++;
  return &ts->transitions[ts->index_prior];
}

/** Set the free agent transition as the most recent prior. */
void atc_transition_storage_set_free_agent_as_prior_if_valid(
    struct AtcTransitionStorage *ts)
{
  struct AtcTransition *ft = ts->transitions[ts->index_free];
  struct AtcTransition *prior = ts->transitions[ts->index_prior];
  if ((prior->is_valid_prior
      && atc_compare_internal_date_time(
          &prior->transition_time,
          &ft->transition_time) < 0)
      || !prior->is_valid_prior) {
    ft->is_valid_prior = true;
    prior->is_valid_prior = false;

    // swap(prior, free)
    ts->transitions[ts->index_prior] = ft;
    ts->transitions[ts->index_free] = prior;
  }
}

/**
 * Add the free agent Transition at index mIndexFree to the Candidate pool,
 * sorted by transitionTime. Then increment mIndexFree by one to remove the
 * free agent from the Free pool. Essentially this is an Insertion Sort
 * keyed by the 'transitionTime' (ignoring the DateTuple.suffix).
 */
void atc_transition_storage_add_free_agent_to_candidate_pool(
    struct AtcTransitionStorage *ts)
{
  if (ts->index_free >= kAtcTransitionStorageSize) return;
  for (uint8_t i= ts->index_free; i > ts->index_candidate; i--) {
    struct AtcTransition *curr = ts->transitions[i];
    struct AtcTransition *prev = ts->transitions[i - 1];
    if (atc_compare_internal_date_time(
        &curr->transition_time,
        &prev->transition_time) >= 0) break;
    ts->transitions[i] = prev;
    ts->transitions[i - 1] = curr;
  }
  ts->index_free++;
}

void atc_transition_storage_add_prior_to_candidate_pool(
    struct AtcTransitionStorage *ts)
{
  ts->index_candidate++;
}

//---------------------------------------------------------------------------
// Step 1
//---------------------------------------------------------------------------

uint8_t atc_processing_find_matches(
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

// Simple Match

void atc_processing_get_transition_time(
    int8_t year_tiny,
    const struct AtcZoneRule* rule,
    struct AtcDateTime *dt)
{
  uint8_t month;
  uint8_t day;
  atc_processing_calc_start_day_of_month(
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

void atc_processing_create_transition_for_year(
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
    atc_processing_get_transition_time(
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
      // will be retrieved by the atc_processing_long_letter() function.
    }
  } else {
    // Create a Transition using the MatchingEra for the transitionTime.
    // Used for simple MatchingEra.
    t->transition_time = match->start_dt;
    t->delta_minutes = atc_zone_info_time_code_to_minutes(
        match->era->delta_code, 0);
  }
}

void atc_processing_create_transitions_from_simple_match(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match)
{
  struct AtcTransition *free_agent = atc_transition_storage_get_free_agent(ts);
  atc_processing_create_transition_for_year(free_agent, 0, NULL, match);
  free_agent->match_status = kAtcMatchStatusExactMatch;
  match->last_offset_minutes = free_agent->offset_minutes;
  match->last_delta_minutes = free_agent->delta_minutes;
  atc_transition_storage_add_free_agent_to_active_pool(ts);
}

//---------------------------------------------------------------------------

// Named Match

/**
 * Calculate interior years. Up to maxInteriorYears, usually 3 or 4.
 * Returns the number of interior years.
 */
uint8_t atc_processing_calc_interior_years(
    int8_t* interior_years,
    uint8_t max_interior_years,
    int8_t from_year,
    int8_t to_year,
    int8_t start_year,
    int8_t end_year)
{
  uint8_t i = 0;
  for (int8_t year = start_year; year <= end_year; year++) {
    if (from_year <= year && year <= to_year) {
      interior_years[i] = year;
      i++;
      if (i >= max_interior_years) break;
    }
  }
  return i;
}

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
 */
uint8_t atc_processing_compare_transition_to_match_fuzzy(
    const struct AtcTransition *t, const struct AtcMatchingEra *match)
{
  int16_t tt_months = t->transition_time.year_tiny * 12
      + t->transition_time.month;

  int16_t match_start_months = match->start_dt.year_tiny * 12
      + match->start_dt.month;
  if (tt_months < match_start_months - 1) return kAtcMatchStatusPrior;

  int16_t match_until_months = match->until_dt.year_tiny * 12
      + match->until_dt.month;
  if (match_until_months + 2 <= tt_months) return kAtcMatchStatusFarFuture;

  return kAtcMatchStatusWithinMatch;
}

int8_t atc_processing_get_most_recent_prior_year(
    int8_t from_year, int8_t to_year,
    int8_t start_year, int8_t end_year)
{
  (void) end_year; // disable compiler warnings

  if (from_year < start_year) {
    if (to_year < start_year) {
      return to_year;
    } else {
      return start_year - 1;
    }
  } else {
    return kAtcInvalidYearTiny;
  }
}

// Pass 1
void atc_processing_find_candidate_transitions(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match)
{
  const struct AtcZonePolicy *policy = match->era->zone_policy;
  uint8_t num_rules = policy->num_rules;
  int8_t start_year_tiny = match->start_dt.year_tiny;
  int8_t end_year_tiny = match->until_dt.year_tiny;

  struct AtcTransition **prior = atc_transition_storage_reserve_prior(ts);
  (*prior)->is_valid_prior = false;
  for (uint8_t r = 0; r < num_rules; r++) {
    const struct AtcZoneRule *rule = &policy->rules[r];

    // Add transitions for interior years
    int8_t interior_years[kAtcMaxInteriorYears];
    uint8_t num_years = atc_processing_calc_interior_years(
        interior_years,
        kAtcMaxInteriorYears,
        rule->from_year_tiny,
        rule->to_year_tiny,
        start_year_tiny,
        end_year_tiny);
    for (uint8_t y = 0; y < num_years; y++) {
      int8_t year = interior_years[y];
      struct AtcTransition *t = atc_transition_storage_get_free_agent(ts);
      atc_processing_create_transition_for_year(t, year,rule, match);
      uint8_t status = atc_processing_compare_transition_to_match_fuzzy(
          t, match);
      if (status == kAtcMatchStatusPrior) {
        atc_transition_storage_set_free_agent_as_prior_if_valid(ts);
      } else if (status == kAtcMatchStatusWithinMatch) {
        atc_transition_storage_add_free_agent_to_candidate_pool(ts);
      } else {
        // Must be kFarFuture.
        // Do nothing, allowing the free agent to be reused.
      }
    }

    // Add Transition for prior year
    int8_t prior_year = atc_processing_get_most_recent_prior_year(
        rule->from_year_tiny, rule->to_year_tiny,
        start_year_tiny, end_year_tiny);
    if (prior_year != kAtcInvalidYearTiny) {
      struct AtcTransition *t = atc_transition_storage_get_free_agent(ts);
      atc_processing_create_transition_for_year(t, prior_year, rule, match);
      atc_transition_storage_set_free_agent_as_prior_if_valid(ts);
    }
  }

  // Add the reserved prior into the Candidate pool only if 'isValidPrior' is
  // true.
  if ((*prior)->is_valid_prior) {
    atc_transition_storage_add_prior_to_candidate_pool(ts);
  }
}

//---------------------------------------------------------------------------
// Pass 2

void atc_processing_fix_transition_times(
      struct AtcTransition **begin,
      struct AtcTransition **end)
{
  (void) begin;
  (void) end;
}

//---------------------------------------------------------------------------
// Pass 3

void atc_processing_select_active_transitions(
      struct AtcTransition **begin,
      struct AtcTransition **end)
{
  (void) begin;
  (void) end;
}

struct AtcTransition *atc_processing_add_active_candidates_to_active_pool(
    struct AtcTransitionStorage *ts)
{
  (void) ts;
  return NULL;
}

//---------------------------------------------------------------------------

void atc_processing_create_transitions_from_named_match(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match)
{
  atc_transition_storage_reset_candidate_pool(ts);

  // Pass 1: Find candidate transitions using whole years.
  atc_processing_find_candidate_transitions(ts, match);

  // Pass 2: Fix the transitions times, converting 's' and 'u' into 'w'
  // uniformly.
  atc_processing_fix_transition_times(
      &ts->transitions[ts->index_candidate],
      &ts->transitions[ts->index_free]);

  // Pass 3: Select only those Transitions which overlap with the actual
  // start and until times of the MatchingEra.
  atc_processing_select_active_transitions(
      &ts->transitions[ts->index_candidate],
      &ts->transitions[ts->index_free]);
  struct AtcTransition *last_transition =
      atc_processing_add_active_candidates_to_active_pool(ts);
  match->last_offset_minutes = last_transition->offset_minutes;
  match->last_delta_minutes = last_transition->delta_minutes;
}

//---------------------------------------------------------------------------

void atc_processing_create_transitions_for_match(
  struct AtcTransitionStorage *ts,
  struct AtcMatchingEra *match)
{
  const struct AtcZonePolicy *policy = match->era->zone_policy;
  if (policy == NULL) {
    atc_processing_create_transitions_from_simple_match(ts, match);
  } else {
    atc_processing_create_transitions_from_named_match(ts, match);
  }
}

void atc_processing_create_transitions(
  struct AtcTransitionStorage *ts,
  struct AtcMatchingEra *matches,
  uint8_t num_matches)
{
  for (uint8_t i = 0; i < num_matches; i++) {
    atc_processing_create_transitions_for_match(ts, &matches[i]);
  }
}

//---------------------------------------------------------------------------
// Initialization of AtcZoneProcessing.
//---------------------------------------------------------------------------

void atc_processing_init(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info)
{
  processing->zone_info = zone_info;
  processing->is_filled = 0;
  processing->num_matches = 0;
}

bool atc_processing_is_filled_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  return processing->is_filled && (year == processing->year);
}

bool atc_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  if (atc_processing_is_filled_for_year(processing, year)) return true;

  processing->year = year;
  processing->num_matches = 0;
  atc_transition_storage_init(&processing->transition_storage);
  const struct AtcZoneContext *context = processing->zone_info->zone_context;
  if (year < context->start_year - 1 || context->until_year < year) {
    return false;
  }
  struct AtcYearMonth start_ym = { (int8_t) (year - kAtcEpochYear - 1), 12 };
  struct AtcYearMonth until_ym = { (int8_t) (year - kAtcEpochYear + 1), 2 };

  // Step 1: Find matches.
  uint8_t num_matches = atc_processing_find_matches(
    processing->zone_info,
    start_ym,
    until_ym,
    processing->matches,
    kAtcMaxMatches);

  // Step 2: Create Transitions.
  atc_processing_create_transitions(
    &processing->transition_storage,
    processing->matches,
    num_matches);

  (void) num_matches;

  // Step 3: Fix transition times.
  // Step 4: Generate start and until times.
  // Step 5: Calc abbreviations.

  return true;
}

void atc_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds)
{
  (void) processing;
  (void) epoch_seconds;
}

//---------------------------------------------------------------------------
// Public API.
//---------------------------------------------------------------------------

void atc_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  atc_time_t epoch_seconds,
  const struct AtcZoneInfo *zone_info,
  struct AtcOffsetDateTime *odt)
{
  (void) processing;
  (void) epoch_seconds;
  (void) zone_info;
  (void) odt;
}

atc_time_t atc_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcLocalDateTime *ldt)
{
  (void) processing;
  (void) ldt;
  return 0;
}
