/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdbool.h>
#include "common.h" // atc_time_t
#include "zone_info.h"
#include "zone_info_utils.h"
#include "local_date.h" // atc_local_date_to_epoch_days()
#include "transition.h"

//---------------------------------------------------------------------------

int8_t atc_date_tuple_compare(
  const AtcDateTuple *a,
  const AtcDateTuple *b)
{
  if (a->year < b->year) return -1;
  if (a->year > b->year) return 1;
  if (a->month < b->month) return -1;
  if (a->month > b->month) return 1;
  if (a->day < b->day) return -1;
  if (a->day > b->day) return 1;
  if (a->minutes < b->minutes) return -1;
  if (a->minutes > b->minutes) return 1;
  return 0;
}

atc_time_t atc_date_tuple_subtract(
    const AtcDateTuple *a,
    const AtcDateTuple *b)
{
  int32_t da = atc_local_date_to_epoch_days(a->year, a->month, a->day);
  int32_t db = atc_local_date_to_epoch_days(b->year, b->month, b->day);

  // Subtract the days, before converting to seconds, to avoid overflowing the
  // int32_t when a.year or b.year is more than 68 years from the
  // atc_current_epoch_year.
  return (da - db) * 86400 + (a->minutes - b->minutes) * 60;
}

void atc_date_tuple_expand(
    const AtcDateTuple *tt,
    int16_t offset_minutes,
    int16_t delta_minutes,
    AtcDateTuple *ttw,
    AtcDateTuple *tts,
    AtcDateTuple *ttu)
{

  if (tt->suffix == kAtcSuffixS) {
    *tts = *tt;

    ttu->year = tt->year;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->minutes = (int16_t) (tt->minutes - offset_minutes);
    ttu->suffix = kAtcSuffixU;

    ttw->year = tt->year;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->minutes = (int16_t) (tt->minutes + delta_minutes);
    ttw->suffix = kAtcSuffixW;
  } else if (tt->suffix == kAtcSuffixU) {
    *ttu = *tt;

    tts->year = tt->year;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->minutes = (int16_t) (tt->minutes + offset_minutes);
    tts->suffix = kAtcSuffixS;

    ttw->year = tt->year;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->minutes = (int16_t) (tt->minutes + (offset_minutes + delta_minutes));
    ttw->suffix = kAtcSuffixW;
  } else {
    // Explicit set the suffix to 'w' in case it was something else.
    *ttw = *tt;
    ttw->suffix = kAtcSuffixW;

    tts->year = tt->year;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->minutes = (int16_t) (tt->minutes - delta_minutes);
    tts->suffix = kAtcSuffixS;

    ttu->year = tt->year;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->minutes = (int16_t) (tt->minutes - (delta_minutes + offset_minutes));
    ttu->suffix = kAtcSuffixU;
  }

  atc_date_tuple_normalize(ttw);
  atc_date_tuple_normalize(tts);
  atc_date_tuple_normalize(ttu);
}

void atc_date_tuple_normalize(AtcDateTuple *dt)
{
  const int16_t kOneDayAsMinutes = 60 * 24;

  if (dt->minutes <= -kOneDayAsMinutes) {
    atc_local_date_decrement_one_day(&dt->year, &dt->month, &dt->day);
    dt->minutes += kOneDayAsMinutes;
  } else if (kOneDayAsMinutes <= dt->minutes) {
    atc_local_date_increment_one_day(&dt->year, &dt->month, &dt->day);
    dt->minutes -= kOneDayAsMinutes;
  } else {
    // do nothing
  }
}

uint8_t atc_date_tuple_compare_fuzzy(
    const AtcDateTuple *t,
    const AtcDateTuple *start,
    const AtcDateTuple *until)
{
  // Use int32_t because a delta year of 2730 or greater will exceed
  // the range of an int16_t.
  int32_t t_months = t->year * (int32_t) 12 + t->month;
  int32_t start_months = start->year * (int32_t) 12 + start->month;
  if (t_months < start_months - 1) return kAtcMatchStatusPrior;
  int32_t until_months = until->year * 12 + until->month;
  if (until_months + 1 < t_months) return kAtcMatchStatusFarFuture;
  return kAtcMatchStatusWithinMatch;
}

//---------------------------------------------------------------------------

void atc_transition_storage_init(
    AtcTransitionStorage *ts, const AtcZoneInfo *zone_info)
{
  ts->zone_info = zone_info;

  for (int i = 0; i < kAtcTransitionStorageSize; i++) {
    ts->transitions[i] = &ts->transition_pool[i];
  }
  ts->index_prior = 0;
  ts->index_candidate = 0;
  ts->index_free = 0;
  ts->alloc_size = 0;
}

AtcTransition **atc_transition_storage_get_candidate_pool_begin(
    AtcTransitionStorage *ts)
{
  return &ts->transitions[ts->index_candidate];
}

AtcTransition **atc_transition_storage_get_candidate_pool_end(
    AtcTransitionStorage *ts)
{
  return &ts->transitions[ts->index_free];
}

AtcTransition **atc_transition_storage_get_active_pool_begin(
    AtcTransitionStorage *ts)
{
  return &ts->transitions[0];
}

AtcTransition **atc_transition_storage_get_active_pool_end(
    AtcTransitionStorage *ts)
{
  // NOTE: I think this should be index_prior not index_free
  return &ts->transitions[ts->index_free];
}

void atc_transition_storage_reset_candidate_pool(
    AtcTransitionStorage *ts)
{
  ts->index_candidate = ts->index_prior;
  ts->index_free = ts->index_prior;
}

AtcTransition *atc_transition_storage_get_free_agent(
    AtcTransitionStorage *ts)
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

void atc_transition_storage_add_free_agent_to_active_pool(
    AtcTransitionStorage *ts)
{
  if (ts->index_free >= kAtcTransitionStorageSize) return;
  ts->index_free++;
  ts->index_prior = ts->index_free;
  ts->index_candidate = ts->index_free;
}

AtcTransition **atc_transition_storage_reserve_prior(
    AtcTransitionStorage *ts)
{
  (void) atc_transition_storage_get_free_agent(ts);
  ts->index_candidate++;
  ts->index_free++;
  return &ts->transitions[ts->index_prior];
}

void atc_transition_storage_set_free_agent_as_prior_if_valid(
    AtcTransitionStorage *ts)
{
  AtcTransition *ft = ts->transitions[ts->index_free];
  AtcTransition *prior = ts->transitions[ts->index_prior];
  if ((prior->is_valid_prior
      && atc_date_tuple_compare(
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

void atc_transition_storage_add_prior_to_candidate_pool(
    AtcTransitionStorage *ts)
{
  ts->index_candidate--;
}

void atc_transition_storage_add_free_agent_to_candidate_pool(
    AtcTransitionStorage *ts)
{
  if (ts->index_free >= kAtcTransitionStorageSize) return;
  for (uint8_t i = ts->index_free; i > ts->index_candidate; i--) {
    AtcTransition *curr = ts->transitions[i];
    AtcTransition *prev = ts->transitions[i - 1];
    if (atc_date_tuple_compare(
        &curr->transition_time,
        &prev->transition_time) >= 0) break;
    ts->transitions[i] = prev;
    ts->transitions[i - 1] = curr;
  }
  ts->index_free++;
}

static bool is_match_status_active(uint8_t status) {
  return status == kAtcMatchStatusExactMatch
      || status == kAtcMatchStatusWithinMatch
      || status == kAtcMatchStatusPrior;
}

AtcTransition *atc_transition_storage_add_active_candidates_to_active_pool(
    AtcTransitionStorage *ts)
{
  // Shift active candidates to the left into the Active pool.
  uint8_t i_active = ts->index_prior;
  uint8_t i_candidate = ts->index_candidate;
  for (; i_candidate < ts->index_free; i_candidate++) {
    if (is_match_status_active(ts->transitions[i_candidate]->match_status)) {
      if (i_active != i_candidate) {
        // Perform swap of pointers to AtcTransition.
        AtcTransition *tmp = ts->transitions[i_active];
        ts->transitions[i_active] = ts->transitions[i_candidate];
        ts->transitions[i_candidate] = tmp;
      }
      ++i_active;
    }
  }

  ts->index_prior = i_active;
  ts->index_candidate = i_active;
  ts->index_free = i_active;

  return ts->transitions[i_active - 1];
}

//---------------------------------------------------------------------------

void atc_transition_fix_times(
    AtcTransition **begin,
    AtcTransition **end)
{
  AtcTransition *prev = *begin;
  for (AtcTransition **iter = begin; iter != end; ++iter) {
    AtcTransition *curr = *iter;
    atc_date_tuple_expand(
        &curr->transition_time,
        prev->offset_minutes,
        prev->delta_minutes,
        &curr->transition_time,
        &curr->transition_time_s,
        &curr->transition_time_u);
    prev = curr;
  }
}

//---------------------------------------------------------------------------

uint8_t atc_transition_compare_to_match(
    const AtcTransition *t, const AtcMatchingEra *match)
{
  // Find the previous Match offsets.
  int16_t prev_match_offset_minutes;
  int16_t prev_match_delta_minutes;
  if (match->prev_match) {
    prev_match_offset_minutes = match->prev_match->last_offset_minutes;
    prev_match_delta_minutes = match->prev_match->last_delta_minutes;
  } else {
    prev_match_offset_minutes = atc_zone_era_std_offset_minutes(match->era);
    prev_match_delta_minutes = 0;
  }

  // Expand start times.
  AtcDateTuple stw;
  AtcDateTuple sts;
  AtcDateTuple stu;
  atc_date_tuple_expand(
      &match->start_dt,
      prev_match_offset_minutes,
      prev_match_delta_minutes,
      &stw,
      &sts,
      &stu);

  // Transition times.
  const AtcDateTuple *ttw = &t->transition_time;
  const AtcDateTuple *tts = &t->transition_time_s;
  const AtcDateTuple *ttu = &t->transition_time_u;

  // Compare Transition to Match, where equality is assumed if *any* of the
  // 'w', 's', or 'u' versions of the DateTuple are equal. This prevents
  // duplicate Transition instances from being created in a few cases.
  if (atc_date_tuple_compare(ttw, &stw) == 0
      || atc_date_tuple_compare(tts, &sts) == 0
      || atc_date_tuple_compare(ttu, &stu) == 0) {
    return kAtcMatchStatusExactMatch;
  }

  if (atc_date_tuple_compare(ttu, &stu) < 0) {
    return kAtcMatchStatusPrior;
  }

  // Now check if the transition occurs after the given match. The
  // untilDateTime of the current match uses the same UTC offsets as the
  // transitionTime of the current transition, so no complicated adjustments
  // are needed. We just make sure we compare 'w' with 'w', 's' with 's',
  // and 'u' with 'u'.
  const AtcDateTuple *match_until = &match->until_dt;
  const AtcDateTuple *transition_time;
  if (match_until->suffix == kAtcSuffixS) {
    transition_time = tts;
  } else if (match_until->suffix == kAtcSuffixU) {
    transition_time = ttu;
  } else { // assume 'w'
    transition_time = ttw;
  }
  if (atc_date_tuple_compare(transition_time, match_until) < 0) {
    return kAtcMatchStatusWithinMatch;
  }

  return kAtcMatchStatusFarFuture;
}

uint8_t atc_transition_compare_to_match_fuzzy(
    const AtcTransition *t, const AtcMatchingEra *match)
{
  return atc_date_tuple_compare_fuzzy(
      &t->transition_time,
      &match->start_dt,
      &match->until_dt);
}

//---------------------------------------------------------------------------

static void atc_calculate_fold_and_overlap(
    uint8_t* fold,
    uint8_t* num,
    const AtcTransition* prev,
    const AtcTransition* curr,
    const AtcTransition* next,
    atc_time_t epoch_seconds)
{
  if (curr == NULL) {
    *fold = 0;
    *num = 0;
    return;
  }

  // Check if within forward overlap shadow from prev
  bool is_overlap;
  if (prev == NULL) {
    is_overlap = false;
  } else {
    // Extract the shift from prev transition. Can be 0 in some cases where
    // the zone changed from DST of one zone to the STD into another zone,
    // causing the overall UTC offset to remain unchanged.
    atc_time_t shift_seconds = atc_date_tuple_subtract(
        &curr->start_dt, &prev->until_dt);
    if (shift_seconds >= 0) {
      // spring forward, or unchanged
      is_overlap = false;
    } else {
      is_overlap = epoch_seconds - curr->start_epoch_seconds < -shift_seconds;
    }
  }
  if (is_overlap) {
    *fold = 1; // selects the second match
    *num = 2;
    return;
  }

  // Check if within backward overlap shawdow from next
  if (next == NULL) {
    is_overlap = false;
  } else {
    // Extract the shift to next transition. Can be 0 in some cases where
    // the zone changed from DST of one zone to the STD into another zone,
    // causing the overall UTC offset to remain unchanged.
    atc_time_t shift_seconds = atc_date_tuple_subtract(
        &next->start_dt, &curr->until_dt);
    if (shift_seconds >= 0) {
      // spring forward, or unchanged
      is_overlap = false;
    } else {
      // Check if within the backward overlap shadow from next
      is_overlap = next->start_epoch_seconds - epoch_seconds <= -shift_seconds;
    }
  }
  if (is_overlap) {
    *fold = 0; // epochSeconds selects the first match
    *num = 2;
    return;
  }

  // Normal single match, no overlap.
  *fold = 0;
  *num = 1;
}

AtcTransitionForSeconds atc_transition_storage_find_for_seconds(
    const AtcTransitionStorage *ts,
    atc_time_t epoch_seconds)
{
  const AtcTransition *prev = NULL;
  const AtcTransition *curr = NULL;
  const AtcTransition *next = NULL;
  for (uint8_t i = 0; i < ts->index_free; i++) {
    next = ts->transitions[i];
    if (next->start_epoch_seconds > epoch_seconds) break;
    prev = curr;
    curr = next;
    next = NULL;
  }

  uint8_t fold;
  uint8_t num;
  atc_calculate_fold_and_overlap(&fold, &num, prev, curr, next, epoch_seconds);
  AtcTransitionForSeconds result = {curr, fold, num};
  return result;
}

AtcTransitionForDateTime atc_transition_storage_find_for_date_time(
    const AtcTransitionStorage *ts,
    const AtcLocalDateTime *ldt)
{
  // Convert LocalDateTime to DateTuple.
  AtcDateTuple local_dt = {
      ldt->year,
      ldt->month,
      ldt->day,
      (int16_t) (ldt->hour * 60 + ldt->minute),
      kAtcSuffixW
  };

  // Examine adjacent pairs of Transitions, looking for an exact match, gap,
  // or overlap.
  const AtcTransition *prev = NULL;
  const AtcTransition *curr = NULL;
  uint8_t num = 0;
  for (uint8_t i = 0; i < ts->index_free; i++) {
    curr = ts->transitions[i];

    const AtcDateTuple *start_dt = &curr->start_dt;
    const AtcDateTuple *until_dt = &curr->until_dt;
    bool is_exact_match = atc_date_tuple_compare(start_dt, &local_dt) <= 0
        && atc_date_tuple_compare(&local_dt, until_dt) < 0;

    if (is_exact_match) {
      // Check for a previous exact match to detect an overlap.
      if (num == 1) {
        num++;
        break;
      }

      // Loop again to detect an overlap.
      num = 1;
    } else if (atc_date_tuple_compare(start_dt, &local_dt) > 0) {
      // Exit loop since no more curr transition.
      break;
    }

    prev = curr;

    // Set nullptr so that if the loop runs off the end of the list of
    // Transitions, the current transition is marked as nullptr.
    curr = NULL;
  }

  // If the prev was an exact match, set curr to the same to avoid confusion.
  if (num == 1) {
    curr = prev;
  }

  AtcTransitionForDateTime result = {prev, curr, num};
  return result;
}
