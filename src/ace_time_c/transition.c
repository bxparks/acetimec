#include "transition.h"
#include "zone_info.h"
#include "zone_info_utils.h"
#include "local_date.h" // atc_local_date_to_epoch_days()

//---------------------------------------------------------------------------

int8_t atc_date_tuple_compare(
  const struct AtcDateTuple *a,
  const struct AtcDateTuple *b)
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

/** Return the number of seconds in (a - b), ignoring suffix. */
atc_time_t atc_date_tuple_subtract(
    const struct AtcDateTuple *a,
    const struct AtcDateTuple *b)
{
  int32_t eda = atc_local_date_to_epoch_days(a->year_tiny, a->month, a->day);
  int32_t esa = eda * 86400 + a->minutes * 60;

  int32_t edb = atc_local_date_to_epoch_days(b->year_tiny, b->month, b->day);
  int32_t esb = edb * 86400 + b->minutes * 60;

  return esa - esb;
}

void atc_date_tuple_expand(
    const struct AtcDateTuple *tt,
    int16_t offset_minutes,
    int16_t delta_minutes,
    struct AtcDateTuple *ttw,
    struct AtcDateTuple *tts,
    struct AtcDateTuple *ttu) {

  if (tt->suffix == kAtcSuffixS) {
    *tts = *tt;

    ttu->year_tiny = tt->year_tiny;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->minutes = (int16_t) (tt->minutes - offset_minutes);
    ttu->suffix = kAtcSuffixU;

    ttw->year_tiny = tt->year_tiny;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->minutes = (int16_t) (tt->minutes + delta_minutes);
    ttw->suffix = kAtcSuffixW;
  } else if (tt->suffix == kAtcSuffixU) {
    *ttu = *tt;

    tts->year_tiny = tt->year_tiny;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->minutes = (int16_t) (tt->minutes + offset_minutes);
    tts->suffix = kAtcSuffixS;

    ttw->year_tiny = tt->year_tiny;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->minutes = (int16_t) (tt->minutes + (offset_minutes + delta_minutes));
    ttw->suffix = kAtcSuffixW;
  } else {
    // Explicit set the suffix to 'w' in case it was something else.
    *ttw = *tt;
    ttw->suffix = kAtcSuffixW;

    tts->year_tiny = tt->year_tiny;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->minutes = (int16_t) (tt->minutes - delta_minutes);
    tts->suffix = kAtcSuffixS;

    ttu->year_tiny = tt->year_tiny;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->minutes = (int16_t) (tt->minutes - (delta_minutes + offset_minutes));
    ttu->suffix = kAtcSuffixU;
  }

  atc_date_tuple_normalize(ttw);
  atc_date_tuple_normalize(tts);
  atc_date_tuple_normalize(ttu);
}

void atc_date_tuple_normalize(struct AtcDateTuple *dt)
{
  const int16_t kOneDayAsMinutes = 60 * 24;

  if (dt->minutes <= -kOneDayAsMinutes) {
    struct AtcLocalDate ld = {
        dt->year_tiny + kAtcEpochYear, dt->month, dt->day};
    atc_local_date_decrement_one_day(&ld);
    dt->year_tiny = ld.year - kAtcEpochYear;
    dt->month = ld.month;
    dt->day = ld.day;
    dt->minutes += kOneDayAsMinutes;
  } else if (kOneDayAsMinutes <= dt->minutes) {
    struct AtcLocalDate ld = {
        dt->year_tiny + kAtcEpochYear, dt->month, dt->day};
    atc_local_date_increment_one_day(&ld);
    dt->year_tiny = ld.year - kAtcEpochYear;
    dt->month = ld.month;
    dt->day = ld.day;
    dt->minutes -= kOneDayAsMinutes;
  } else {
    // do nothing
  }
}

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
    if (atc_date_tuple_compare(
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

static bool is_match_status_active(uint8_t status) {
  return status == kAtcMatchStatusExactMatch
      || status == kAtcMatchStatusWithinMatch
      || status == kAtcMatchStatusPrior;
}

struct AtcTransition *
atc_transition_storage_add_active_candidates_to_active_pool(
    struct AtcTransitionStorage *ts)
{
  // Shift active candidates to the left into the Active pool.
  uint8_t i_active = ts->index_prior;
  uint8_t i_candidate = ts->index_candidate;
  for (; i_candidate < ts->index_free; i_candidate++) {
    if (is_match_status_active(ts->transitions[i_candidate]->match_status)) {
      if (i_active != i_candidate) {
        // Perform swap of pointers to AtcTransition.
        struct AtcTransition *tmp = ts->transitions[i_active];
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

const char *atc_transition_extract_letter(const struct AtcTransition *t)
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
  const struct AtcZonePolicy *policy = t->match->era->zone_policy;
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
    const struct AtcTransition *t, const struct AtcMatchingEra *match)
{
  // Find the previous Match offsets.
  int16_t prev_match_offset_minutes;
  int16_t prev_match_delta_minutes;
  if (match->prev_match) {
    prev_match_offset_minutes = match->prev_match->last_offset_minutes;
    prev_match_delta_minutes = match->prev_match->last_delta_minutes;
  } else {
    prev_match_offset_minutes = atc_zone_info_time_code_to_minutes(
        match->era->offset_code, 0);
    prev_match_delta_minutes = 0;
  }

  // Expand start times.
  struct AtcDateTuple stw;
  struct AtcDateTuple sts;
  struct AtcDateTuple stu;
  atc_date_tuple_expand(
      &match->start_dt,
      prev_match_offset_minutes,
      prev_match_delta_minutes,
      &stw,
      &sts,
      &stu);

  // Transition times.
  const struct AtcDateTuple *ttw = &t->transition_time;
  const struct AtcDateTuple *tts = &t->transition_time_s;
  const struct AtcDateTuple *ttu = &t->transition_time_u;

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
  const struct AtcDateTuple *match_until = &match->until_dt;
  const struct AtcDateTuple *transition_time;
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
