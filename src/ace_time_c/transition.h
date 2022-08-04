/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_TRANSITION_H
#define ACE_TIME_C_TRANSITION_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h" // atc_time_t

//---------------------------------------------------------------------------

/**
 * An internal simplified version of the AtcDateTime class that uses the
 * (year_tiny, month, day, minutes, suffix) fields.
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
struct AtcDateTuple {
  int8_t year_tiny; // [-127, 126], 127 will cause bugs
  uint8_t month; // [1-12]
  uint8_t day; // [1-31]
  int16_t minutes; // negative values allowed
  uint8_t suffix; // kAtcSuffixS, kAtcSuffixW, kAtcSuffixU
};

/** Compare a to b, ignoring the suffix. Exported for testing. */
int8_t atc_date_tuple_compare(
    const struct AtcDateTuple *a,
    const struct AtcDateTuple *b);

/** Return (a - b) in number of seconds, ignoring the suffix. */
atc_time_t atc_date_tuple_subtract(
    const struct AtcDateTuple *a,
    const struct AtcDateTuple *b);

void atc_date_tuple_expand(
    const struct AtcDateTuple *tt,
    int16_t offset_minutes,
    int16_t delta_minutes,
    struct AtcDateTuple *ttw,
    struct AtcDateTuple *tts,
    struct AtcDateTuple *ttu);

void atc_date_tuple_normalize(struct AtcDateTuple *dt);

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
struct AtcMatchingEra {
  /**
   * The effective start time of the matching ZoneEra, which uses the
   * UTC offsets of the previous matching era.
   */
  struct AtcDateTuple start_dt;

  /** The effective until time of the matching ZoneEra. */
  struct AtcDateTuple until_dt;

  /** The ZoneEra that matched the given year. NonNullable. */
  struct AtcZoneEra *era;

  /** The previous MatchingEra, needed to interpret start_dt.  */
  struct AtcMatchingEra *prev_match;

  /** The STD offset of the last Transition in this MatchingEra. */
  int16_t last_offset_minutes;

  /** The DST offset of the last Transition in this MatchingEra. */
  int16_t last_delta_minutes;
};

struct AtcTransition {
  /** The matching_era which generated this Transition. */
  const struct AtcMatchingEra *match;

  /**
   * The Zone transition rule that matched for the the given year. Set to
   * nullptr if the RULES column is '-', indicating that the MatchingEra was
   * a "simple" ZoneEra.
   */
  const struct AtcZoneRule *rule;

  /**
   * The original transition time, usually 'w' but sometimes 's' or 'u'. After
   * expandDateTuple() is called, this field will definitely be a 'w'. We must
   * remember that the transition_time* fields are expressed using the UTC
   * offset of the *previous* Transition.
   */
  struct AtcDateTuple transition_time;

  union {
    /**
     * Version of transition_time in 's' mode, using the UTC offset of the
     * *previous* Transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple transition_time_s;

    /**
     * Start time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple start_dt;
  };

  union {
    /**
     * Version of transition_time in 'u' mode, using the UTC offset of the
     * *previous* transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple transition_time_u;

    /**
     * Until time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTuple until_dt;
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
  char letter_buf[2];

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
};

/** The list of transitions for a given time zone. */
struct AtcTransitionStorage {
  struct AtcTransition transition_pool[kAtcTransitionStorageSize];
  struct AtcTransition *transitions[kAtcTransitionStorageSize];
  uint8_t index_prior;
  uint8_t index_candidate;
  uint8_t index_free;

  /** Number of allocated transitions. */
  uint8_t alloc_size;
};

//---------------------------------------------------------------------------

void atc_transition_storage_init(struct AtcTransitionStorage *ts);

struct AtcTransition *atc_transition_storage_get_free_agent(
    struct AtcTransitionStorage *ts);

void atc_transition_storage_add_free_agent_to_active_pool(
    struct AtcTransitionStorage *ts);

void atc_transition_storage_reset_candidate_pool(
    struct AtcTransitionStorage *ts);

struct AtcTransition **atc_transition_storage_reserve_prior(
    struct AtcTransitionStorage *ts);

void atc_transition_storage_set_free_agent_as_prior_if_valid(
    struct AtcTransitionStorage *ts);

void atc_transition_storage_add_free_agent_to_candidate_pool(
    struct AtcTransitionStorage *ts);

void atc_transition_storage_add_prior_to_candidate_pool(
    struct AtcTransitionStorage *ts);

struct AtcTransition *
atc_transition_storage_add_active_candidates_to_active_pool(
    struct AtcTransitionStorage *ts);

/**
 * Return the letter string. Returns NULL if the RULES column is empty
 * since that means that the ZoneRule is not used, which means LETTER does
 * not exist. A LETTER of '-' is returned as an empty string "".
 */
const char *atc_transition_extract_letter(const struct AtcTransition *t);

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
    const struct AtcTransition *t, const struct AtcMatchingEra *match);

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
    const struct AtcTransition *t, const struct AtcMatchingEra *match);

#endif
