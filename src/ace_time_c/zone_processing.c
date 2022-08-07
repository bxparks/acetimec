/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdio.h> //printf()
#include <stdbool.h>
#include <string.h> // memcpy(), strncpy()
#include "common.h" // atc_copy_replace_string()
#include "local_date.h" // atc_local_date_days_in_year_month()
#include "zone_info_utils.h"
#include "offset_date_time.h"
#include "zone_processing.h"

//---------------------------------------------------------------------------

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
static bool atc_era_overlaps_interval(
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
    const struct AtcZoneEra *era,
    struct AtcYearMonth start_ym,
    struct AtcYearMonth until_ym) {

  // If prev_match is null, set start_date to be earlier than all valid
  // ZoneEra.
  struct AtcDateTuple start_date;
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
    start_date.minutes = atc_zone_era_until_minutes(prev_match->era);
    start_date.suffix = atc_zone_era_until_suffix(prev_match->era);
  }
  struct AtcDateTuple lower_bound = {
    start_ym.year_tiny,
    start_ym.month,
    1,
    0,
    kAtcSuffixW
  };
  if (atc_date_tuple_compare(&start_date, &lower_bound) < 0) {
    start_date = lower_bound;
  }

  struct AtcDateTuple until_date = {
    era->until_year_tiny,
    era->until_month,
    era->until_day,
    atc_zone_era_until_minutes(era),
    atc_zone_era_until_suffix(era),
  };
  struct AtcDateTuple upper_bound = {
    until_ym.year_tiny,
    until_ym.month,
    1,
    0,
    kAtcSuffixW
  };
  if (atc_date_tuple_compare(&upper_bound, &until_date) < 0) {
    until_date = upper_bound;
  }

  new_match->start_dt = start_date;
  new_match->until_dt = until_date;
  new_match->era = era;
  new_match->prev_match = prev_match;
  new_match->last_offset_minutes = 0;
  new_match->last_delta_minutes = 0;
}

struct AtcMonthDay atc_processing_calc_start_day_of_month(
    int16_t year,
    uint8_t month,
    uint8_t on_day_of_week,
    int8_t on_day_of_month)
{
  struct AtcMonthDay md;

  if (on_day_of_week == 0) {
    md.month = month;
    md.day = on_day_of_month;
    return md;
  }

  if (on_day_of_month >= 0) {
    uint8_t days_in_month = atc_local_date_days_in_year_month(year, month);
    if (on_day_of_month == 0) {
      on_day_of_month = days_in_month - 6;
    }
    uint8_t dow = atc_local_date_day_of_week(year, month, on_day_of_month);
    uint8_t day_of_week_shift = (on_day_of_week - dow + 7) % 7;
    uint8_t day = (uint8_t) (on_day_of_month + day_of_week_shift);
    if (day > days_in_month) {
      // TODO: Support shifting from Dec to Jan of following  year.
      day -= days_in_month;
      month++;
    }
    md.month = month;
    md.day = day;
    return md;
  } else {
    on_day_of_month = -on_day_of_month;
    uint8_t dow = atc_local_date_day_of_week(year, month, on_day_of_month);
    int8_t day_of_week_shift = (dow - on_day_of_week + 7) % 7;
    int8_t day = on_day_of_month - day_of_week_shift;
    if (day < 1) {
      // TODO: Support shifting from Jan to Dec of the previous year.
      month--;
      uint8_t days_in_prev_month = atc_local_date_days_in_year_month(
          year, month);
      day += days_in_prev_month;
    }
    md.month = month;
    md.day = day;
    return md;
  }
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
// Step 2A: Simple Match
// ---------------------------------------------------------------------------

void atc_processing_get_transition_time(
    int8_t year_tiny,
    const struct AtcZoneRule* rule,
    struct AtcDateTuple *dt)
{
  struct AtcMonthDay md = atc_processing_calc_start_day_of_month(
      year_tiny + kAtcEpochYear,
      rule->in_month,
      rule->on_day_of_week,
      rule->on_day_of_month);

  dt->year_tiny = year_tiny;
  dt->month = md.month;
  dt->day = md.day;
  dt->minutes = atc_zone_rule_at_minutes(rule);
  dt->suffix = atc_zone_rule_at_suffix(rule);
}

void atc_processing_create_transition_for_year(
    struct AtcTransition *t,
    int8_t year_tiny,
    const struct AtcZoneRule *rule,
    const struct AtcMatchingEra *match)
{
  t->match = match;
  t->rule = rule;
  t->offset_minutes = atc_zone_era_dst_offset_minutes(match->era);
  t->letter_buf[0] = '\0';

  if (rule) {
    atc_processing_get_transition_time(
        year_tiny, rule, &t->transition_time);
    t->delta_minutes = atc_zone_rule_dst_offset_minutes(rule);
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
    t->delta_minutes = atc_zone_era_dst_offset_minutes(match->era);
  }
}

void atc_processing_create_transitions_from_simple_match(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match)
{
  printf("***atc_processing_create_transitions_from_simple_match()\n");
  struct AtcTransition *free_agent = atc_transition_storage_get_free_agent(ts);
  atc_processing_create_transition_for_year(free_agent, 0, NULL, match);
  free_agent->match_status = kAtcMatchStatusExactMatch;
  match->last_offset_minutes = free_agent->offset_minutes;
  match->last_delta_minutes = free_agent->delta_minutes;
  atc_transition_storage_add_free_agent_to_active_pool(ts);
}

//---------------------------------------------------------------------------
// Step 2B: Pass 1
//---------------------------------------------------------------------------

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
      uint8_t status = atc_transition_compare_to_match_fuzzy(
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
// Step 2B: Pass 3
//---------------------------------------------------------------------------

static void atc_processing_process_transition_match_status(
    struct AtcTransition *transition,
    struct AtcTransition **prior)
{
  uint8_t status = atc_transition_compare_to_match(
      transition, transition->match);
  transition->match_status = status;

  if (status == kAtcMatchStatusExactMatch) {
    if (*prior) {
      (*prior)->match_status = kAtcMatchStatusFarPast;
    }
    (*prior) = transition;
  } else if (status == kAtcMatchStatusPrior) {
    if (*prior) {
      if (atc_date_tuple_compare(
          &(*prior)->transition_time_u,
          &transition->transition_time_u) <= 0) {
        (*prior)->match_status = kAtcMatchStatusFarPast;
        (*prior) = transition;
      } else {
        transition->match_status = kAtcMatchStatusFarPast;
      }
    } else {
      (*prior) = transition;
    }
  }
}

void atc_processing_select_active_transitions(
    struct AtcTransition **begin,
    struct AtcTransition **end)
{
  struct AtcTransition *prior = NULL;
  for (struct AtcTransition **iter = begin; iter != end; ++iter) {
    struct AtcTransition *transition = *iter;
    atc_processing_process_transition_match_status(transition, &prior);
  }

  // If the latest prior transition is found, shift it to start at the
  // startDateTime of the current match.
  if (prior) {
    prior->transition_time = prior->match->start_dt;
  }
}

//---------------------------------------------------------------------------
// Step 2B
//---------------------------------------------------------------------------

void atc_processing_create_transitions_from_named_match(
    struct AtcTransitionStorage *ts,
    struct AtcMatchingEra *match)
{
  printf("***atc_processing_create_transitions_from_named_match()\n");
  atc_transition_storage_reset_candidate_pool(ts);

  // Pass 1: Find candidate transitions using whole years.
  atc_processing_find_candidate_transitions(ts, match);

  // Pass 2: Fix the transitions times, converting 's' and 'u' into 'w'
  // uniformly.
  atc_transition_fix_times(
      &ts->transitions[ts->index_candidate],
      &ts->transitions[ts->index_free]);

  // Pass 3: Select only those Transitions which overlap with the actual
  // start and until times of the MatchingEra.
  atc_processing_select_active_transitions(
      &ts->transitions[ts->index_candidate],
      &ts->transitions[ts->index_free]);
  struct AtcTransition *last_transition =
      atc_transition_storage_add_active_candidates_to_active_pool(ts);
  match->last_offset_minutes = last_transition->offset_minutes;
  match->last_delta_minutes = last_transition->delta_minutes;
}

//---------------------------------------------------------------------------
// Step 2
//---------------------------------------------------------------------------

void atc_processing_create_transitions_for_match(
  struct AtcTransitionStorage *ts,
  struct AtcMatchingEra *match)
{
  const struct AtcZonePolicy *policy = match->era->zone_policy;
  if (policy == NULL) {
    // Step 2A
    atc_processing_create_transitions_from_simple_match(ts, match);
  } else {
    // Step 2B
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
// Step 4
//---------------------------------------------------------------------------
void atc_processing_generate_start_until_times(
    struct AtcTransition **begin,
    struct AtcTransition **end)
{
  struct AtcTransition *prev = *begin;
  bool is_after_first = false;

  for (struct AtcTransition **iter = begin; iter != end; ++iter) {
    struct AtcTransition * const t = *iter;

    // 1) Update the untilDateTime of the previous Transition
    const struct AtcDateTuple *tt = &t->transition_time;
    if (is_after_first) {
      prev->until_dt = *tt;
    }

    // 2) Calculate the current startDateTime by shifting the
    // transitionTime (represented in the UTC offset of the previous
    // transition) into the UTC offset of the *current* transition.
    int16_t minutes = tt->minutes + (
        - prev->offset_minutes - prev->delta_minutes
        + t->offset_minutes + t->delta_minutes);
    t->start_dt.year_tiny = tt->year_tiny;
    t->start_dt.month = tt->month;
    t->start_dt.day = tt->day;
    t->start_dt.minutes = minutes;
    t->start_dt.suffix = tt->suffix;
    atc_date_tuple_normalize(&t->start_dt);

    // 3) The epochSecond of the 'transitionTime' is determined by the
    // UTC offset of the *previous* Transition. However, the
    // transitionTime can be represented by an illegal time (e.g. 24:00).
    // So, it is better to use the properly normalized startDateTime
    // (calculated above) with the *current* UTC offset.
    //
    // NOTE: We should also be able to  calculate this directly from
    // 'transitionTimeU' which should still be a valid field, because it
    // hasn't been clobbered by 'untilDateTime' yet. Not sure if this saves
    // any CPU time though, since we still need to mutiply by 900.
    const struct AtcDateTuple *st = &t->start_dt;
    const atc_time_t offset_seconds = (atc_time_t) 60
        * (st->minutes - (t->offset_minutes + t->delta_minutes));
    int32_t epoch_seconds = (int32_t) 86400 * atc_local_date_to_epoch_days(
        st->year_tiny, st->month, st->day);
    t->start_epoch_seconds = epoch_seconds + offset_seconds;

    prev = t;
    is_after_first = true;
  }

  // The last Transition's until time is the until time of the MatchingEra.
  struct AtcDateTuple until_time_w;
  struct AtcDateTuple until_time_s;
  struct AtcDateTuple until_time_u;
  atc_date_tuple_expand(
      &prev->match->until_dt,
      prev->offset_minutes,
      prev->delta_minutes,
      &until_time_w,
      &until_time_s,
      &until_time_u);
  prev->until_dt = until_time_w;
}


//---------------------------------------------------------------------------
// Step 5
//---------------------------------------------------------------------------

static void atc_processing_create_abbreviation(
    char* dest,
    uint8_t dest_size,
    const char* format,
    uint16_t delta_minutes,
    const char* letter_string) {

  // Check if FORMAT contains a '%'.
  if (strchr(format, '%') != NULL) {
    // Check if RULES column empty, therefore no 'letter'
    if (letter_string == NULL) {
      strncpy(dest, format, dest_size - 1);
      dest[dest_size - 1] = '\0';
    } else {
      atc_copy_replace_string(
          dest, dest_size, format, '%', letter_string);
    }
  } else {
    // Check if FORMAT contains a '/'.
    const char* slash_pos = strchr(format, '/');
    if (slash_pos != NULL) {
      if (delta_minutes == 0) {
        uint8_t head_length = (slash_pos - format);
        if (head_length >= dest_size) head_length = dest_size - 1;
        memcpy(dest, format, head_length);
        dest[head_length] = '\0';
      } else {
        uint8_t tail_length = strlen(slash_pos+1);
        if (tail_length >= dest_size) tail_length = dest_size - 1;
        memcpy(dest, slash_pos+1, tail_length);
        dest[tail_length] = '\0';
      }
    } else {
      // Just copy the FORMAT disregarding delta_minutes and letter_string.
      strncpy(dest, format, dest_size);
      dest[dest_size - 1] = '\0';
    }
  }
}

void atc_processing_calc_abbreviations(
    struct AtcTransition **begin,
    struct AtcTransition **end)
{
  for (struct AtcTransition **iter = begin; iter != end; ++iter) {
    struct AtcTransition * const t = *iter;
    atc_processing_create_abbreviation(
        t->abbrev,
        kAtcAbbrevSize,
        t->match->era->format,
        t->delta_minutes,
        atc_transition_extract_letter(t));
  }
}

//---------------------------------------------------------------------------
// Initialization of AtcZoneProcessing.
//---------------------------------------------------------------------------

void atc_processing_init(struct AtcZoneProcessing *processing)
{
  processing->zone_info = NULL;
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
  const struct AtcZoneInfo *zone_info,
  int16_t year)
{
  if (processing->zone_info != zone_info) {
    atc_processing_init(processing);
    processing->zone_info = zone_info;
  }
  printf("***atc_processing_init_for_year(): #1\n");
  if (atc_processing_is_filled_for_year(processing, year)) return true;

  processing->year = year;
  processing->num_matches = 0;
  atc_transition_storage_init(&processing->transition_storage);
  const struct AtcZoneContext *context = processing->zone_info->zone_context;
  printf("***atc_processing_init_for_year(): #2\n");
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
  printf("***atc_processing_init_for_year(): num_matches=%d\n", num_matches);

  // Step 2: Create Transitions.
  atc_processing_create_transitions(
    &processing->transition_storage,
    processing->matches,
    num_matches);

  // Step 3: Fix transition times.
  struct AtcTransitionStorage *ts = &processing->transition_storage;
  struct AtcTransition **begin = &ts->transitions[0];
  struct AtcTransition **end = &ts->transitions[ts->index_free];
  atc_transition_fix_times(begin, end);

  // Step 4: Generate start and until times.
  atc_processing_generate_start_until_times(begin, end);

  // Step 5: Calc abbreviations.
  atc_processing_calc_abbreviations(begin, end);

  printf("***atc_processing_init_for_year(): #3\n");
  return true;
}

bool atc_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds)
{
  if (processing->zone_info != zone_info) {
    atc_processing_init(processing);
    processing->zone_info = zone_info;
  }

  struct AtcLocalDateTime ldt;
  atc_local_date_time_from_epoch_seconds(epoch_seconds, &ldt);
  return atc_processing_init_for_year(processing, zone_info, ldt.year);
}

//---------------------------------------------------------------------------

struct AtcMatchingTransition {
  const struct AtcTransition *transition;
  uint8_t fold; // 1 if in the overlap, otherwise 0
};

static uint8_t atc_processing_calculate_fold(
    atc_time_t epoch_seconds,
    const struct AtcTransition *match,
    const struct AtcTransition *prev_match)
{
  if (match == NULL) return 0;
  if (prev_match == NULL) return 0;

  // Check if epoch_seconds occurs during a "fall back" DST transition.
  atc_time_t overlap_seconds = atc_date_tuple_subtract(
      &prev_match->until_dt, &match->start_dt);
  if (overlap_seconds <= 0) return 0;
  atc_time_t seconds_from_transition_start =
      epoch_seconds - match->start_epoch_seconds;
  if (seconds_from_transition_start >= overlap_seconds) return 0;

  // EpochSeconds occurs within the "fall back" overlap.
  return 1;
}

static struct AtcMatchingTransition atc_processing_find_transition_for_seconds(
    const struct AtcTransitionStorage *ts,
    atc_time_t epoch_seconds)
{
  const struct AtcTransition *prev_match = NULL;
  const struct AtcTransition *match = NULL;
  for (uint8_t i = 0; i < ts->index_free; i++) {
    const struct AtcTransition *candidate = ts->transitions[i];
    if (candidate->start_epoch_seconds > epoch_seconds) break;
    prev_match = match;
    match = candidate;
  }
  uint8_t fold = atc_processing_calculate_fold(
      epoch_seconds, match, prev_match);
  struct AtcMatchingTransition result = { match, fold };
  return result;
}

static struct AtcTransitionResult atc_processing_find_transition_for_date_time(
    const struct AtcTransitionStorage *ts,
    const struct AtcLocalDateTime *ldt)
{
  // Convert LocalDateTime to DateTuple.
  struct AtcDateTuple local_dt = {
      ldt->year - kAtcEpochYear,
      ldt->month,
      ldt->day,
      (int16_t) (ldt->hour * 60 + ldt->minute),
      kAtcSuffixW
  };

  // Examine adjacent pairs of Transitions, looking for an exact match, gap,
  // or overlap.
  const struct AtcTransition *prev_candidate = NULL;
  const struct AtcTransition *candidate = NULL;
  int8_t search_status = kAtcSearchStatusGap;
  for (uint8_t i = 0; i < ts->index_free; i++) {
    candidate = ts->transitions[i];

    const struct AtcDateTuple *start_dt = &candidate->start_dt;
    const struct AtcDateTuple *until_dt = &candidate->until_dt;
    bool is_exact_match = atc_date_tuple_compare(start_dt, &local_dt) <= 0
        && atc_date_tuple_compare(&local_dt, until_dt) < 0;

    if (is_exact_match) {
      // Check for a previous exact match to detect an overlap.
      if (search_status == kAtcSearchStatusExact) {
        search_status = kAtcSearchStatusOverlap;
        break;
      }

      // Loop again to detect an overlap.
      search_status = kAtcSearchStatusExact;

    } else if (atc_date_tuple_compare(start_dt, &local_dt) > 0) {
      // Exit loop since no more candidate transition.
      break;
    }

    prev_candidate = candidate;

    // Set nullptr so that if the loop runs off the end of the list of
    // Transitions, the candidate is marked as nullptr.
    candidate = NULL;
  }

  // Check if the prev was an exact match, and clear the current to
  // avoid confusion.
  if (search_status == kAtcSearchStatusExact) {
    candidate = NULL;
  }

  struct AtcTransitionResult result = {
    prev_candidate,
    candidate,
    search_status,
  };
  return result;
}

//---------------------------------------------------------------------------

bool atc_processing_offset_date_time_from_epoch_seconds(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *odt)
{
  bool status = atc_processing_init_for_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds);
  printf("***atc_processing_offset_date_time_from_epoch_seconds(): #1\n");
  if (! status) return status;

  struct AtcMatchingTransition mt = atc_processing_find_transition_for_seconds(
      &processing->transition_storage, epoch_seconds);
  const struct AtcTransition *t = mt.transition;
  printf("***atc_processing_offset_date_time_from_epoch_seconds(): #2: t=%d\n",
    (t != NULL));
  if (! t) return false;

  atc_local_date_time_from_epoch_seconds(
      epoch_seconds,
      (struct AtcLocalDateTime *) odt);
  odt->offset_minutes = t->offset_minutes + t->delta_minutes;
  odt->fold = mt.fold;
  printf("***atc_processing_offset_date_time_from_epoch_seconds(): #3\n");
  return true;
}

bool atc_processing_offset_date_time_from_local_date_time(
  struct AtcZoneProcessing *processing,
  const struct AtcZoneInfo *zone_info,
  const struct AtcLocalDateTime *ldt,
  uint8_t fold,
  struct AtcOffsetDateTime *odt)
{
  bool status = atc_processing_init_for_year(processing, zone_info, ldt->year);
  if (! status) return status;

  struct AtcTransitionResult result =
      atc_processing_find_transition_for_date_time(
          &processing->transition_storage, ldt);

  // Extract the appropriate Transition, depending on the requested 'fold'
  // and the 'result.search_status'.
  bool needs_normalization = false;
  const struct AtcTransition *t;
  if (result.search_status == kAtcSearchStatusExact) {
    t = result.transition0;
    printf("***atc_processing_offset_date_time_from_local_date_time(): #1\n");
  } else {
    if (result.transition0 == NULL || result.transition1 == NULL) {
      // ldt was far past or far future, and didn't match anything.
      t = NULL;
      printf("***atc_processing_offset_date_time_from_local_date_time(): t=NULL\n");
    } else {
      needs_normalization = (result.search_status == kAtcSearchStatusGap);
      t = (fold == 0) ? result.transition0 : result.transition1;
      printf("***atc_processing_offset_date_time_from_local_date_time(): #2\n");
    }
  }

  if (! t) return false;

  odt->year = ldt->year;
  odt->month = ldt->month;
  odt->day = ldt->day;
  odt->hour = ldt->hour;
  odt->minute = ldt->minute;
  odt->second = ldt->second;
  odt->offset_minutes = t->offset_minutes + t->delta_minutes;
  odt->fold = fold;

  if (needs_normalization) {
    atc_time_t epoch_seconds = atc_offset_date_time_to_epoch_seconds(odt);

    // If in the gap, normalization means that we convert to epochSeconds
    // then perform another search through the Transitions, then use
    // that new Transition to convert the epochSeconds to OffsetDateTime. It
    // turns out that this process identical to just using the other
    // Transition returned in TransitionResult above.
    const struct AtcTransition *othert = (fold == 0)
        ? result.transition1
        : result.transition0;
    atc_local_date_time_from_epoch_seconds(
        epoch_seconds, (struct AtcLocalDateTime *) odt);
    odt->offset_minutes = othert->offset_minutes + othert->delta_minutes;

    // Invert the fold.
    // 1) The normalization process causes the LocalDateTime to jump to the
    // other transition.
    // 2) Provides a user-accessible indicator that a gap normalization was
    // performed.
    odt->fold = 1 - fold;
  }

  return true;
}
