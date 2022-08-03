#include "transition.h"
#include "zone_info.h"
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

//---------------------------------------------------------------------------

/**
  * Return the letter string. Returns NULL if the RULES column is empty
  * since that means that the ZoneRule is not used, which means LETTER does
  * not exist. A LETTER of '-' is returned as an empty string "".
  */
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

