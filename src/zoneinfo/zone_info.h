/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zone_info.h
 *
 * The data structures in the zone database, with the AtcZoneInfo representing
 * a specific time zone.
 */

#ifndef ACE_TIME_C_ZONE_INFO_H
#define ACE_TIME_C_ZONE_INFO_H

#include <stddef.h>
#include <stdint.h>

#ifndef ATC_HIRES_ZONEDB
/** Set to 1 to use high-resolution (i.e. 1-second resolution) data fields. */
#define ATC_HIRES_ZONEDB 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

/** Constants for entries in the zonedb files. */
enum {
  /**
   * The minimum value of AtcZoneRule::from_year and AtcZoneRule::to_year. Used
   * by synthetic entries for certain zones, to guarantee that all zones have at
   * least one transition.
   */
  kAtcMinZoneRuleYear = -32767,

  /**
   * The maximum value of AtcZoneRule::from_year and AtcZoneRule::to_year,
   * representing the sentinel value "max" in the TO and FROM columns of the
   * TZDB files. Must be less than kAtcMaxZoneEraUntilYear.
   */
  kAtcMaxZoneRuleYear = 32766,

  /**
   * The maximum value of ZoneEra::until_year, representing the sentinel value
   * "-" in the UNTIL column of the TZDB files. Must be greater than
   * kAtcMaxZoneRuleYear.
   */
  kAtcMaxZoneEraUntilYear = kAtcMaxZoneRuleYear + 1,
};

//---------------------------------------------------------------------------

/**
 * A time zone transition rule. It is useful to think of this as a transition
 * rule that repeats on the given (month, day, hour) every year during the
 * interval [fromYear, toYear] inclusive.
 */
typedef struct AtcZoneRule {

  /** FROM year. */
  int16_t const from_year;

  /** TO year. */
  int16_t const to_year;

  /** Determined by the IN column. 1=Jan, 12=Dec. */
  uint8_t const in_month;

  /**
   * Determined by the ON column. Possible values are: 0, 1=Mon, 7=Sun.
   * There are 4 combinations:
   * @verbatim
   * on_day_of_week=0, on_day_of_month=(1-31): exact match
   * on_day_of_week=1-7, on_day_of_month=1-31: day_of_week>=on_day_of_month
   * on_day_of_week=1-7, on_day_of_month=-(1-31): day_of_week<=on_day_of_month
   * on_day_of_week=1-7, on_day_of_month=0: last{day_of_week}
   * @endverbatim
   */
  uint8_t const on_day_of_week;

  /**
   * Determined by the ON column. Used with on_day_of_week. Possible values are:
   * 0, 1-31, or its corresponding negative values.
   */
  int8_t const on_day_of_month;

#if ATC_HIRES_ZONEDB

  /**
   * The at_time_modifier is a packed field containing 2 pieces of info:
   *
   * * The upper 4 bits represent the AT time suffix: 'w', 's' or 'u',
   *   represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   * * The lower 4 bits represent the remaining 0-14 seconds of the AT time
   *   field after truncation into at_time_seconds_code. In other words, the
   *   full AT field in one-second resolution is (15 * at_time_seconds_code +
   *   (at_time_modifier & 0x0f)).
   */
  uint8_t const at_time_modifier;

  /**
   * Determined by the AT column in units of 15-seconds from 00:00. The range
   * corresponds to [0h,25h], or [0,6000] in 15-second units.
   */
  uint16_t const at_time_code;

  /**
   * Determined by the SAVE column, containing the offset from UTC in minutes,
   * in the range of [-128,+127].
   */
  int8_t const delta_minutes;

#else

  /**
   * Determined by the AT column in units of 15-minutes from 00:00. The range
   * is (0 - 100) corresponding to 00:00 to 25:00.
   */
  uint8_t const at_time_code;

  /**
   * The at_time_modifier is a packed field containing 2 pieces of info:
   *
   * * The upper 4 bits represent the AT time suffix: 'w', 's' or 'u',
   *   represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   * * The lower 4 bits represent the remaining 0-14 minutes of the AT field
   *   after truncation into at_time_code. In other words, the full AT field in
   *   one-minute resolution is (15 * at_time_code + (at_time_modifier & 0x0f)).
   */
  uint8_t const at_time_modifier;

  /**
   * Determined by the SAVE column and contains the offset from UTC, in 15-min
   * increments. The deltaCode is equal to (original_delta_code + 4). Only the
   * lower 4-bits is used, for consistency with the AtcZoneEra.delta_code field.
   * This allows the 4-bits to represent DST offsets from -1:00 to 2:45 in
   * 15-minute increments.
   *
   * The atc_zone_rule_dst_offset_minutes() function knows how to convert this
   * field into minutes.
   */
  uint8_t const delta_code;

#endif

  /**
   * An index into an array of strings defined by AtcZoneContext.letters. The
   * string comes from Rule.LETTER column. Most letter values are single
   * character strings (e.g. "S", "D", and ""). But a small number of zones have
   * LETTER columsn with multiple characters (e.g. "CST", "+00"). The string is
   * substituted into the '%s' field (implemented in acetimee by just a '%') of
   * the AtcZoneInfo.format field (e.g. "P%T", "M%T").
   *
   * As of TZ DB version 2018i, 4 ZonePolicies have ZoneRules with a LETTER
   * field longer than 1 character:
   *
   *  - Belize ('CST'; used by America/Belize)
   *  - Namibia ('WAT', 'CAT'; used by Africa/Windhoek)
   *  - StJohns ('DD'; used by America/St_Johns and America/Goose_Bay)
   *  - Troll ('+00' '+02'; used by Antarctica/Troll)
   */
  uint8_t const letter_index;
} AtcZoneRule;

/**
 * A collection of transition rules which describe the DST rules of a given
 * administrative region. A given time zone (ZoneInfo) can follow a different
 * ZonePolicy at different times. Conversely, multiple time zones (ZoneInfo)
 * can choose to follow the same ZonePolicy at different times.
 *
 * If num_letters is non-zero, then 'letters' will be a pointer to an array of
 * (const char*) pointers. Any ZoneRule.letter < 32 (i.e. non-printable) will
 * be an offset into this array of pointers.
 */
typedef struct AtcZonePolicy {
  /** Pointer to array of rules. */
  const AtcZoneRule* const rules;

  /** Number of rules in array. */
  uint8_t const num_rules;
} AtcZonePolicy;

//---------------------------------------------------------------------------

enum {
  /** Represents 'w' or wall time. */
  kAtcSuffixW = 0x00,

  /** Represents 's' or standard time. */
  kAtcSuffixS = 0x10,

  /** Represents 'u' or UTC time. */
  kAtcSuffixU = 0x20,
};

/** Information about the zone database. */
typedef struct AtcZoneContext {
  /** Start year of the zone files as requested. */
  int16_t start_year;

  /** Until year of the zone files as requested. */
  int16_t until_year;

  /**
   * Start year of accurate transitions. kAtcMinZoneRuleYear indicates
   * -Infinity
   */
  int16_t start_year_accurate;

  /**
   * Until year of accurate transitions. kAtcMaxZoneEraUntilYear indicates
   * +Infinity
   */
  int16_t until_year_accurate;

  /** The maximum transitions required in TransitionStorage. */
  int16_t max_transitions;

  /** TZ Database version which generated the zone info. */
  const char *tz_version;

  /** Number of fragments. */
  uint8_t num_fragments;

  /** Number of letters. */
  uint8_t num_letters;

  /** Zone Name fragment list. */
  const char * const *fragments;

  /** Zone letters list (e.g. "", "D", "S"). */
  const char * const *letters;
} AtcZoneContext;

//---------------------------------------------------------------------------

/**
 * An entry in ZoneInfo which describes which ZonePolicy was being followed
 * during a particular time period. Corresponds to one line of the ZONE record
 * in the TZ Database file ending with an UNTIL field. The ZonePolicy is
 * determined by the RULES column in the TZ Database file.
 *
 * There are 2 types of ZoneEra:
 *    1) zone_policy == nullptr. Then delta_code determines the additional
 *    offset from offset_code. A value of '-' in the TZ Database file is stored
 *    as 0.
 *    2) zone_policy != nullptr. Then the delta_code offset is given by the
 *    ZoneRule.delta_code of the ZoneRule which matches the time instant of
 *    interest.
 */
typedef struct AtcZoneEra {
  /**
   * Zone policy, determined by the RULES column. Set to nullptr if the RULES
   * column is '-' or an explicit DST shift in the form of 'hh:mm'.
   */
  const AtcZonePolicy * const zone_policy;

  /**
   * Zone abbreviations (e.g. PST, EST) determined by the FORMAT column. It has
   * 3 encodings in the TZ DB files:
   *
   *  1) A fixed string, e.g. "GMT".
   *  2) Two strings separated by a '/', e.g. "-03/-02" indicating
   *     "{std}/{dst}" options.
   *  3) A single string with a substitution, e.g. "E%sT", where the "%s" is
   *  replaced by the LETTER value from the ZoneRule.
   *
   * BasicZoneProcessor supports only a single letter subsitution from LETTER,
   * but ExtendedZoneProcessor supports substituting multi-character strings
   * (e.g. "CAT", "DD", "+00").
   *
   * The TZ DB files use '%s' to indicate the substitution, but for simplicity,
   * AceTime replaces the "%s" with just a '%' character with no loss of
   * functionality. This also makes the string-replacement code a little
   * simpler. For example, 'E%sT' is stored as 'E%T', and the LETTER
   * substitution is performed on the '%' character.
   *
   * This field will never be a 'nullptr' if it was derived from an actual
   * entry from the TZ database. There is an internal object named
   * `ExtendedZoneProcessor::kAnchorEra` which does set this field to nullptr.
   * Maybe it should be set to ""?
   */
  const char * const format;

#if ATC_HIRES_ZONEDB

  /**
   * UTC offset seconds in units of 15-seconds. The remainder goes into
   * offset_seconds_remainder.
   */
  int16_t const offset_code;

  /** UTC offset seconds remainder from the offset_seconds_code. */
  uint8_t const offset_remainder;

  /**
   * If zone_policy is nullptr, then this indicates the DST offset in minutes
   * as defined by the RULES column in 'hh:mm' format. If the
   * 'RULES' column is '-', then the delta_minutes is 0.
   */
  int8_t const delta_minutes;

#else

  /** UTC offset in 15 min increments. Determined by the STDOFF column. */
  int8_t const offset_code;

  /**
   * This is a composite of two 4-bit fields:
   *
   * * The upper 4-bits is an unsigned integer from 0 to 14 that represents
   *   the one-minute remainder from the offset_code. This allows us to capture
   *   STDOFF offsets in 1-minute resolution.
   * * The lower 4-bits is an unsigned integer that holds (original_delta_code
   *   + 4). The original_delta_code is defined if zone_policy is NULL, which
   *   indicates that the DST offset is defined by the RULES column in 'hh:mm'
   *   format. If the 'RULES' column is '-', then the original_delta_code is 0.
   *   With 4-bits of information, and the 1h shift, this allows us to represent
   *   DST offsets from -1:00 to +2:45, in 15-minute increments.
   *
   * The atc_zone_era_std_offset_minutes() and atc_zone_era_dst_offset_minutes()
   * functions know how to convert offset_code and delta_code into the
   * appropriate minutes.
   */
  uint8_t const delta_code;

#endif

  /**
   * Era is valid until currentTime < untilYear. Comes from the UNTIL column.
   */
  int16_t const until_year;

  /** The month field in UNTIL (1-12). Will never be 0. */
  uint8_t const until_month;

  /**
   * The day field in UNTIL (1-31). Will never be 0. Also, there's no need for
   * untilDayOfWeek, because the database generator will resolve the exact day
   * of month based on the known year and month.
   */
  uint8_t const until_day;

#if ATC_HIRES_ZONEDB

  /**
   * The time field of UNTIL field in 15-second increments. A range is [0h,25h],
   * corresponding to [0,6000] in units of 15-seconds.
   */
  uint16_t const until_time_code;

  /**
   * The until_time_modifier is a packed field containing 2 pieces of info:
   *
   * * The upper 4 bits represent the UNTIL time suffix: 'w', 's' or 'u',
   *   represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   * * The lower 4 bits represent the remaining 0-14 seconds of the UNTIL
   *   field after truncation into until_time_seconds_code. In other words, the
   *   full UNTIL field in one-second resolution is (15 *
   *   until_time_seconds_code + (until_time_modifier & 0x0f)).
   */
  uint8_t const until_time_modifier;
#else

  /**
   * The time field of UNTIL field in 15-minute increments. A range of 00:00 to
   * 25:00 corresponds to 0-100.
   */
  uint8_t const until_time_code;

  /**
   * The until_time_modifier is a packed field containing 2 pieces of info:
   *
   * * The upper 4 bits represent the UNTIL time suffix: 'w', 's' or 'u',
   *   represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   * * The lower 4 bits represent the remaining 0-14 minutes of the UNTIL
   *   field after truncation into untilTimeCode. In other words, the full
   *   UNTIL field in one-minute resolution is (15 * until_time_code +
   *   (until_time_modifier & 0x0f)).
   */
  uint8_t const until_time_modifier;

#endif
} AtcZoneEra;

/**
 * Representation of a given time zone, implemented as an array of ZoneEra
 * records.
 */
typedef struct AtcZoneInfo {
  /** Full name of zone (e.g. "America/Los_Angeles"). */
  const char * const name;

  /**
   * Unique, stable ID of the zone name, created from a hash of the name.
   * This ID will never change once assigned. This can be used for presistence
   * and serialization.
   */
  uint32_t const zone_id;

  /** ZoneContext metadata. */
  const AtcZoneContext * const zone_context;

  /** Number of ZoneEra entries. Set to 0 if this Zone is a actually a Link. */
  uint8_t const num_eras;

  /**
   * A `const ZoneEras*` pointer or a `const ZoneInfo*` pointer. For a normal
   * Zone, numEras is greater than 0, and this field is a pointer to the
   * ZoneEra entries in increasing order of UNTIL time. For a Link entry,
   * numEras == 0, and this field provides a level of indirection to a (const
   * ZoneInfo*) pointer to the target Zone. We have to follow the indirect
   * pointer, and resolve the target numEras and eras to obtain the actual
   * ZoneEra entries.
   */
  const AtcZoneEra * const eras;

  /** A pointer to an AtcZoneInfo for a Link. NULL otherwise. */
  const struct AtcZoneInfo * target_info;
} AtcZoneInfo;

//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
