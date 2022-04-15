/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_PROCESSING_H
#define ACE_TIME_C_ZONE_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h" // atc_time_t
#include "zone_info.h"

//---------------------------------------------------------------------------
// Conversion and accessor utilities.
//---------------------------------------------------------------------------

extern inline uint16_t atc_zone_info_time_code_to_minutes(
  uint8_t code, uint8_t modifier)
{
  return code * (uint16_t) 15 + (modifier & 0x0f);
}

extern inline uint8_t atc_zone_info_modifier_to_suffix(uint8_t modifier)
{
  return modifier & 0xf0;
}

//---------------------------------------------------------------------------
// Data structures to track ZoneEra transitions and associated info.
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
};

/** A tuple of (year_tiny, month). */
struct AtcYearMonth {
  int8_t year_tiny;
  uint8_t month;
};

/** An internal tuple of (year_tiny, month, day, minutes). */
struct AtcDateTime {
  int8_t year_tiny; // [-127, 126], 127 will cause bugs
  uint8_t month; // [1-12]
  uint8_t day; // [1-31]
  uint8_t suffix; // kAtcSuffixS, kAtcSuffixW, kAtcSuffixU
  int16_t minutes; // negative values allowed
};

/** A struct that represents a matching ZoneEra. */
struct AtcMatchingEra {
  /**
   * The effective start time of the matching ZoneEra, which uses the
   * UTC offsets of the previous matching era.
   */
  struct AtcDateTime start_dt;

  /** The effective until time of the matching ZoneEra. */
  struct AtcDateTime until_dt;

  /** The ZoneEra that matched the given year. NonNullable. */
  struct AtcZoneEra *era;

  /** The previous MatchingEra, needed to interpret start_dt.  */
  struct AtcMatchingEra *prev_match;

  /** The STD offset of the last Transition in this MatchingEra. */
  uint16_t last_offset_minutes;

  /** The DST offset of the last Transition in this MatchingEra. */
  uint16_t last_delta_minutes;
};

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

struct AtcTransition {
  /** The matching_era which generated this Transition. */
  const struct AtcMatchingEra *match;

  /**
   * The Zone transition rule that matched for the the given year. Set to
   * nullptr if the RULES column is '-', indicating that the MatchingEra was
   * a "simple" ZoneEra.
   */
  struct AtcZoneRule *rule;

  /**
   * The original transition time, usually 'w' but sometimes 's' or 'u'. After
   * expandDateTuple() is called, this field will definitely be a 'w'. We must
   * remember that the transition_time* fields are expressed using the UTC
   * offset of the *previous* Transition.
   */
  struct AtcDateTime transition_time;

  union {
    /**
     * Version of transition_time in 's' mode, using the UTC offset of the
     * *previous* Transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTime transition_time_s;

    /**
     * Start time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTime start_dt;
  };

  union {
    /**
     * Version of transition_time in 'u' mode, using the UTC offset of the
     * *previous* transition. Valid before
     * ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTime transition_time_u;

    /**
     * Until time expressed using the UTC offset of the current Transition.
     * Valid after ExtendedZoneProcessor::generateStartUntilTimes() is called.
     */
    struct AtcDateTime until_dt;
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

struct AtcTransitionResult {
  const struct AtcTransition *transition0; // fold==0
  const struct AtcTransition *transition1; // fold==1
  int8_t search_status;
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
// Externally exported API for converting between epoch seconds and
// LocalDateTime and OffsetDateTime.
//---------------------------------------------------------------------------

/**
 * Zone processing work space. One of these should be created for each active
 * timezone. It can be reused among multiple timezones but a change of timezone
 * causes the internal cache to be wiped and recreated.
 */
struct AtcZoneProcessing {
  const struct AtcZoneInfo *zone_info;
  int16_t year; // maybe create LocalDate::kInvalidYear?
  uint8_t is_filled;
  uint8_t num_matches; // actual number of matches
  struct AtcMatchingEra matches[kAtcMaxMatches];
  struct AtcTransitionStorage transition_storage;
};

struct AtcLocalDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  int16_t offset_minutes;
};

void atc_zone_processing_init(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info);

bool atc_zone_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year);

void atc_zone_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds);

void atc_zone_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *offset_dt);

atc_time_t atc_zone_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  struct AtcLocalDateTime *local_dt);

#endif
