/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file transition.h
 *
 * Data structures and functions related to calculating DST transitions.
 */

#ifndef ACE_TIME_C_TRANSITION_H
#define ACE_TIME_C_TRANSITION_H

#include <stdint.h>
#include "../zoneinfo/zone_info.h" // AtcZoneEra
#include "common.h" // atc_time_t
#include "local_date_time.h" // AtcLocalDateTime

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

/**
 * An internal simplified version of the AtcDateTime class that uses the
 * (year, month, day, minutes, suffix) fields.
 *
 * The order of 'minutes' and 'suffix' are reversed from the DateTuple class in
 * the AceTime library because C does not support constructors, so the
 * initializer list must be in the same order as the field order. But I have a
 * bunch of code ported from the C++ code in AceTime which assumes that
 * 'minutes' comes before 'suffix'. So it is easier to fix the ordering here.
 *
 * This causes the field alignment to be slightly less than ideal, but the class
 * already consumes 2 x 4-byte slots on 32-bit processors, so the new ordering
 * does not change the overall sizeof(AtcDateTuple).
 */
typedef struct AtcDateTuple {
  /** [0,10000] */
  int16_t year;

  /** [1-12] */
  uint8_t month;

  /** [1-31] */
  uint8_t day;

  /** negative values allowed */
  int16_t minutes;

  /** kAtcSuffixS, kAtcSuffixW, kAtcSuffixU */
  uint8_t suffix;
} AtcDateTuple;

/** Compare a to b, ignoring the suffix. */
int8_t atc_date_tuple_compare(
    const AtcDateTuple *a,
    const AtcDateTuple *b);

/** Return (a - b) in number of seconds, ignoring the suffix. */
atc_time_t atc_date_tuple_subtract(
    const AtcDateTuple *a,
    const AtcDateTuple *b);

/** Normalize AtcDateTuple::minutes if its magnitude is more than 24 hours. */
void atc_date_tuple_normalize(AtcDateTuple *dt);

/**
 * Convert the given 'tt', offsetMinutes, and deltaMinutes into the 'w', 's'
 * and 'u' versions of the AtcDateTuple. It is allowed for 'ttw' to be an alias
 * of 'tt'.
 */
void atc_date_tuple_expand(
    const AtcDateTuple *tt,
    int16_t offset_minutes,
    int16_t delta_minutes,
    AtcDateTuple *ttw,
    AtcDateTuple *tts,
    AtcDateTuple *ttu);

/**
 * Compare the given 't' with the interval defined by [start, until). The
 * comparison is fuzzy, with a slop of about one month so that we can ignore the
 * day and minutes fields.
 *
 * The following values are returned:
 *
 *  * kAtcMatchStatusPrior if 't' is less than 'start' by at least one month,
 *  * kAtcMatchStatusFarFuture if 't' is greater than 'until' by at least one
 *    month,
 *  * kAtcMatchStatusWithinMatch if 't' is within [start, until) with a one
 *    month slop,
 *  * kAtcMatchStatusExactMatch is never returned.
 */
uint8_t atc_date_tuple_compare_fuzzy(
    const AtcDateTuple *t,
    const AtcDateTuple *start,
    const AtcDateTuple *until);

//---------------------------------------------------------------------------

enum {
  /**
   * Number of characters in a time zone abbreviation. 6 is the maximum
   * plus 1 for the NUL terminator.
   */
  kAtcAbbrevSize = 7,

  /** Number of transitions in the transition_storage. */
  kAtcTransitionStorageSize = 8,

  /**
   * Number of Matches. We look at the 3 years straddling the current year, plus
   * the most recent prior year, so that makes 4.
   */
  kAtcMaxMatches = 4,

  /**
   * Maximum number of interior years. For a viewing window of 14 months,
   * this will be 4. (Verify: I think this can be changed to 3.)
   */
  kAtcMaxInteriorYears = 4,
};

/** A struct that represents a matching ZoneEra. */
typedef struct AtcMatchingEra {
  /**
   * The effective start time of the matching ZoneEra, which uses the
   * UTC offsets of the previous matching era.
   */
  AtcDateTuple start_dt;

  /** The effective until time of the matching ZoneEra. */
  AtcDateTuple until_dt;

  /** The ZoneEra that matched the given year. NonNullable. */
  const AtcZoneEra *era;

  /** The previous MatchingEra, needed to interpret start_dt.  */
  struct AtcMatchingEra *prev_match;

  /** The STD offset of the last Transition in this MatchingEra. */
  int16_t last_offset_minutes;

  /** The DST offset of the last Transition in this MatchingEra. */
  int16_t last_delta_minutes;
} AtcMatchingEra;

/**
 * A transition from one DST rule to another, and the time period which
 * that rule is valid for.
 */
typedef struct AtcTransition {
  /** The matching_era which generated this Transition. */
  const AtcMatchingEra *match;

  /**
   * The Zone transition rule that matched for the the given year. Set to
   * nullptr if the RULES column is '-', indicating that the MatchingEra was
   * a "simple" ZoneEra.
   */
  const AtcZoneRule *rule;

  /**
   * The original transition time, usually 'w' but sometimes 's' or 'u'. After
   * expandDateTuple() is called, this field will definitely be a 'w'. We must
   * remember that the transition_time* fields are expressed using the UTC
   * offset of the *previous* Transition.
   */
  AtcDateTuple transition_time;

  union {
    /**
     * Version of transition_time in 's' mode, using the UTC offset of the
     * *previous* Transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    AtcDateTuple transition_time_s;

    /**
     * Start time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    AtcDateTuple start_dt;
  };

  union {
    /**
     * Version of transition_time in 'u' mode, using the UTC offset of the
     * *previous* transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    AtcDateTuple transition_time_u;

    /**
     * Until time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    AtcDateTuple until_dt;
  };

  /** The calculated transition time of the given rule. */
  atc_time_t start_epoch_seconds;

  /**
   * The base offset minutes, not the total effective UTC offset. Note that
   * this is different than basic::Transition::offsetMinutes used by
   * BasicZoneProcessor which is the total effective offsetMinutes. (It may be
   * possible to make this into an effective offsetMinutes (i.e. offsetMinutes
   * + deltaMinutes) but it does not seem worth making that change right now.)
   */
  int16_t offset_minutes;

  /** The DST delta minutes. */
  int16_t delta_minutes;

  /** The calculated effective time zone abbreviation, e.g. "PST" or "PDT". */
  char abbrev[kAtcAbbrevSize];

  /** Storage for the single letter 'letter' field if 'rule' is not null. */
  const char *letter;

  union {
    /**
     * During findCandidateTransitions(), this flag indicates whether the
     * current transition is a valid "prior" transition that occurs before other
     * transitions. It is set by setFreeAgentAsPriorIfValid() if the transition
     * is a prior transition.
     */
    uint8_t is_valid_prior;

    /**
     * During processTransitionMatchStatus(), this flag indicates how the
     * transition falls within the time interval of the MatchingEra.
     */
    uint8_t match_status;
  };
} AtcTransition;

//---------------------------------------------------------------------------

/** The list of transitions for a given time zone. */
typedef struct AtcTransitionStorage {
  /**
   * Pointer to the AtcZoneInfo that this TransitionStorage describes. Needed
   * to access the AtcZoneInfo.letters array.
   */
  const AtcZoneInfo *zone_info;

  /** A pool of AtcTransition objects. */
  AtcTransition transition_pool[kAtcTransitionStorageSize];
  /** Pointers into the pool of AtcTransition objects. */
  AtcTransition *transitions[kAtcTransitionStorageSize];
  /** Index of the most recent prior transition [0,kAtcTransitionStorageSize) */
  uint8_t index_prior;
  /** Index of the candidate pool [0,kAtcTransitionStorageSize) */
  uint8_t index_candidate;
  /** Index of the free agent transition [0, kAtcTransitionStorageSize) */
  uint8_t index_free;

  /** Number of allocated transitions. */
  uint8_t alloc_size;
} AtcTransitionStorage;

/** Initialize the Transition Storage. Should be called once for a given app. */
void atc_transition_storage_init(
    AtcTransitionStorage *ts, const AtcZoneInfo* zone_info);

/** Return the begin iterator of the 'candidate' transitions. */
AtcTransition **atc_transition_storage_get_candidate_pool_begin(
    AtcTransitionStorage *ts);

/** Return the end iterator of the 'candidate' transitions. */
AtcTransition **atc_transition_storage_get_candidate_pool_end(
    AtcTransitionStorage *ts);

/** Return the begin iterator of the 'active' transitions. */
AtcTransition **atc_transition_storage_get_active_pool_begin(
    AtcTransitionStorage *ts);

/** Return the end iterator of the 'active' transitions. */
AtcTransition **atc_transition_storage_get_active_pool_end(
    AtcTransitionStorage *ts);

/**
 * Return a pointer to the first Transition in the free pool. If this
 * transition is not used, then it's ok to just drop it. The next time
 * getFreeAgent() is called, the same Transition will be returned.
 */
AtcTransition *atc_transition_storage_get_free_agent(
    AtcTransitionStorage *ts);

/**
 * Immediately add the free agent Transition at index mIndexFree to the
 * Active pool. Then increment mIndexFree to consume the free agent
 * from the Free pool. This assumes that the Pending and Candidate pool are
 * empty, which makes the Active pool come immediately before the Free
 * pool.
 */
void atc_transition_storage_add_free_agent_to_active_pool(
    AtcTransitionStorage *ts);

/**
 * Empty the Candidate pool by resetting the various indexes.
 *
 * If every iteration of createTransitionsForMatch() finishes with
 * addFreeAgentToActivePool() or addActiveCandidatesToActivePool(), it may
 * be possible to remove this. But it's safer to reset the indexes upon
 * each iteration.
 */
void atc_transition_storage_reset_candidate_pool(
    AtcTransitionStorage *ts);

/**
 * Allocate a free Transition then add it to the Prior pool. This assumes
 * that the Prior pool and Candidate pool were both empty before calling
 * this method. Shift the Candidate pool and Free pool up by one. Return a
 * handle (pointer to pointer) to the Transition, so that the prior
 * Transition can be swapped with another Transition, while keeping the
 * handle valid.
 */
AtcTransition **atc_transition_storage_reserve_prior(
    AtcTransitionStorage *ts);

/** Set the free agent transition as the most recent prior. */
void atc_transition_storage_set_free_agent_as_prior_if_valid(
    AtcTransitionStorage *ts);

/**
 * Add the free agent Transition at index mIndexFree to the Candidate pool,
 * sorted by transitionTime. Then increment mIndexFree by one to remove the
 * free agent from the Free pool. Essentially this is an Insertion Sort
 * keyed by the 'transitionTime' (ignoring the DateTuple.suffix).
 */
void atc_transition_storage_add_free_agent_to_candidate_pool(
    AtcTransitionStorage *ts);

/**
 * Add the current prior into the Candidates pool. Prior is always just
 * before the start of the Candidate pool, so we just need to shift back
 * the start index of the Candidate pool.
 */
void atc_transition_storage_add_prior_to_candidate_pool(
    AtcTransitionStorage *ts);

/**
 * Add active candidates into the Active pool, and collapse the Candidate
 * pool. Every MatchingEra will have at least one Transition.
 *
 * @return the last Transition that was added
 */
AtcTransition *atc_transition_storage_add_active_candidates_to_active_pool(
    AtcTransitionStorage *ts);

//---------------------------------------------------------------------------

/**
 * Normalize the transition_time* fields of the array of Transition objects.
 * Most Transition.transition_time* values are given in 'w' mode. However, if
 * they are given in 's' or 'u' mode, we convert these into the 'w' mode for
 * consistency. To convert an 's' or 'u' into 'w', we need the UTC offset of the
 * current AtcTransition, which happens to be given by the *previous*
 * AtcTransition.
 */
void atc_transition_fix_times(
    AtcTransition **begin,
    AtcTransition **end);

//---------------------------------------------------------------------------

/**
 * The result of comparing transition of a Transition to the time interval
 * of the corresponding AtcMatchingEra.
 */
enum {
  kAtcMatchStatusFarPast, // 0
  kAtcMatchStatusPrior, // 1
  kAtcMatchStatusExactMatch, // 2
  kAtcMatchStatusWithinMatch, // 3
  kAtcMatchStatusFarFuture, // 4
};

/**
 * Compare the temporal location of transition compared to the interval
 * defined by  the match. The transition time of the Transition is expanded
 * to include all 3 versions ('w', 's', and 'u') of the time stamp. When
 * comparing against the MatchingEra.startDateTime and
 * MatchingEra.untilDateTime, the version will be determined by the suffix
 * of those parameters.
 */
uint8_t atc_transition_compare_to_match(
    const AtcTransition *t, const AtcMatchingEra *match);

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
uint8_t atc_transition_compare_to_match_fuzzy(
    const AtcTransition *t, const AtcMatchingEra *match);

//---------------------------------------------------------------------------

/**
 * The result returned by atc_transition_storage_find_for_seconds() when
 * searching for Transition by epoch seconds. Searching by epoch_seconds is
 * guaranteed to return only a single Transition if found. Usually `fold=0`. But
 * if the epoch_seconds maps to a AtcLocalDateTime which occurs a second time
 * during a "fall back", then `fold` is set to 1.
 *
 * Adapted from TransitionForSeconds in Transition.h of the AceTime library.
 */
typedef struct AtcTransitionForSeconds {
  /** The matching transition or null if not found. */
  const AtcTransition *curr;

  /** 0 for the first or exact transition; 1 for the second transition */
  uint8_t fold;

  /**
   * Number of occurrences of the resulting AtcLocalDateTime: 0, 1, or 2.
   * This is needed because a fold=0 can mean that the AtcLocalDateTime occurs
   * exactly once, or that the first of two occurrences of AtcLocalDateTime was
   * selected by the epoch_seconds.
   */
  uint8_t num;
} AtcTransitionForSeconds;

/**
 * Find the Transition matching the given epochSeconds. Return
 * AtcTransitionForSeconds.curr == nullptr if no matching Transition found. If a
 * zone does not have any transition according to TZ Database, the
 * AceTimeTools/transformer.py script adds an "anchor" transition at the
 * "beginning of time" which happens to be the year 1872 (because the year is
 * stored as an int8_t). Therefore, this method should never return a nullptr
 * for a well-formed ZoneInfo file.
 */
AtcTransitionForSeconds atc_transition_storage_find_for_seconds(
    const AtcTransitionStorage *ts,
    atc_time_t epoch_seconds);

/**
 * The result returned by atc_transition_storage_find_for_date_time() when
 * searching for transitions by local date time. There are 5 possibilities:
 *
 *  * num=0, prev==NULL, curr=curr: datetime is far past
 *  * num=1, prev==prev, curr=prev: exact match to datetime
 *  * num=2, prev==prev, curr=curr: datetime in overlap
 *  * num=0, prev==prev, curr=curr: datetime in gap
 *  * num=0, prev==prev, curr=NULL: datetime is far future
 *
 * Adapted from TransitionForDateTime in Transition.h of the AceTime library.
 */
typedef struct AtcTransitionForDateTime {
  /** The previous transition, or null if the first transition matches. */
  const AtcTransition *prev;

  /** The matching transition or null if not found. */
  const AtcTransition *curr;

  /** Number of matches for given LocalDateTime: 0, 1, or 2. */
  uint8_t num;
} AtcTransitionForDateTime;

/**
 * Return the candidate Transitions matching the given dateTime. The search may
 * return 0, 1 or 2 Transitions, depending on whether the dateTime falls in a
 * gap or overlap.
 */
AtcTransitionForDateTime atc_transition_storage_find_for_date_time(
    const AtcTransitionStorage *ts,
    const AtcLocalDateTime *ldt);

#ifdef __cplusplus
}
#endif

#endif
