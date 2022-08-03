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

/** An internal tuple of (year_tiny, month, day, minutes). */
struct AtcDateTuple {
  int8_t year_tiny; // [-127, 126], 127 will cause bugs
  uint8_t month; // [1-12]
  uint8_t day; // [1-31]
  uint8_t suffix; // kAtcSuffixS, kAtcSuffixW, kAtcSuffixU
  int16_t minutes; // negative values allowed
};

/** Compare a to b, ignoring the suffix. Exported for testing. */
int8_t atc_date_tuple_compare(
    const struct AtcDateTuple *a,
    const struct AtcDateTuple *b);

/** Return (a - b) in number of seconds, ignoring the suffix. */
atc_time_t atc_date_tuple_subtract(
    const struct AtcDateTuple *a,
    const struct AtcDateTuple *b);

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
  uint16_t last_offset_minutes;

  /** The DST offset of the last Transition in this MatchingEra. */
  uint16_t last_delta_minutes;
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

const char *atc_transition_extract_letter(const struct AtcTransition *t);

#endif
