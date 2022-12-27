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
  int32_t eda = atc_local_date_to_epoch_days(a->year, a->month, a->day);
  int32_t esa = eda * 86400 + a->minutes * 60;

  int32_t edb = atc_local_date_to_epoch_days(b->year, b->month, b->day);
  int32_t esb = edb * 86400 + b->minutes * 60;

  return esa - esb;
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

void atc_transition_storage_init(AtcTransitionStorage *ts)
{
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

const char *atc_transition_extract_letter(const AtcTransition *t)
{
  // RULES column is '-' or hh:mm, so return NULL to indicate this.
  if (t->rule == NULL) {
    return NULL;
  }

  // RULES point to a named rule, and LETTER is a single, printable character.
  // Return the letter_buf which contains a NUL-terminated string containing the
  // single character, as initialized in
  // atc_processing_create_transition_for_year().
  char letter = t->rule->letter;
  if (letter >= 32) {
    return t->letter_buf;
  }

  // RULES points to a named rule, and the LETTER is a string. The
  // rule->letter is a non-printable number < 32, which is an index into
  // a list of strings given by match->era->zonePolicy->letters[].
  const AtcZonePolicy *policy = t->match->era->zone_policy;
  uint8_t num_letters = policy->num_letters;
  if (letter >= num_letters) {
    // This should never happen unless there is a programming error. If it
    // does, return an empty string. (createTransitionForYear() sets
    // letterBuf to a NUL terminated empty string if rule->letter < 32)
    return t->letter_buf;
  }

  // Return the string at index 'rule->letter'.
  return policy->letters[(uint8_t) letter];
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
