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

//---------------------------------------------------------------------------

/**
 * Define an empty ACE_TIME_C_PROGMEM. May be used in the future to support
 * storing zonedb in flash memory on AVR processors.
 */
#define ACE_TIME_C_PROGMEM

/** Constants for entries in the zonedb files. */
enum {
  /**
   * The maximum value of ZoneRule::from_year_tiny and ZoneRule::to_year_tiny.
   * Must be < ZoneEra::kMaxUntilYear.
   */
  kAtcMaxZoneRuleYearTiny = 126,

  /** The maximum value of ZoneEra::until_year_tiny. */
  kAtcMaxZoneEraUntilYearTiny = kAtcMaxZoneRuleYearTiny + 1,
};

//---------------------------------------------------------------------------

/**
 * A time zone transition rule. It is useful to think of this as a transition
 * rule that repeats on the given (month, day, hour) every year during the
 * interval [fromYear, toYear] inclusive.
 */
struct AtcZoneRule {

  /** FROM year as an offset from year 2000 stored as a single byte. */
  int8_t const from_year_tiny;

  /** TO year as an offset from year 2000 stored as a single byte. */
  int8_t const to_year_tiny;

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

  /**
   * Determined by the AT column in units of 15-minutes from 00:00. The range
   * is (0 - 100) corresponding to 00:00 to 25:00.
   */
  uint8_t const at_time_code;

  /**
   * The at_time_modifier is a packed field containing 2 pieces of info:
   *
   *    * The upper 4 bits represent the AT time suffix: 'w', 's' or 'u',
   *    represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   *    * The lower 4 bits represent the remaining 0-14 minutes of the AT field
   *    after truncation into at_time_code. In other words, the full AT field in
   *    one-minute resolution is (15 * at_time_code + (at_time_modifier &
   *    0x0f)).
   */
  uint8_t const at_time_modifier;

  /**
   * Determined by the SAVE column, containing the offset from UTC, in 15-min
   * increments.
   *
   * If the '--scope extended' flag is given to tzcompiler.py, this field
   * should be interpreted as an uint8_t field, whose lower 4-bits hold a
   * slightly modified value of offset_code equal to (originalDeltaCode + 4).
   * This allows the 4-bits to represent DST offsets from -1:00 to 2:45 in
   * 15-minute increments. This is the same algorithm used by
   * ZoneEra::delta_code field for consistency. The
   * extended::ZonePolicyBroker::deltaMinutes() method knows how to convert
   * this field into minutes.
   */
  int8_t const delta_code;

  /**
   * Determined by the LETTER column. Determines the substitution into the '%s'
   * field (implemented here by just a '%') of the ZoneInfo::format field.
   * Possible values are 'S', 'D', '-', or a number < 32 (i.e. a non-printable
   * character). If the value is < 32, then this number is an index offset into
   * the ZonePolicy.letters[] array which contains a (const char*) of the
   * actual multi-character letter.
   *
   * BasicZoneProcessor supports only a single LETTER value (i.e. >= 32), which
   * also means that ZonePolicy.num_letters will always be 0 for a
   * BasicZoneProcessor. ExtendedZoenProcessor supports a LETTER value of < 32,
   * indicating a multi-character string.
   *
   * As of TZ DB version 2018i, there are 4 ZonePolicies which have ZoneRules
   * with a LETTER field longer than 1 character:
   *
   *  - Belize ('CST'; used by America/Belize)
   *  - Namibia ('WAT', 'CAT'; used by Africa/Windhoek)
   *  - StJohns ('DD'; used by America/St_Johns and America/Goose_Bay)
   *  - Troll ('+00' '+02'; used by Antarctica/Troll)
   */
  uint8_t const letter;
};

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
struct AtcZonePolicy {
  /** Pointer to array of rules. */
  const struct AtcZoneRule* const rules;

  /** Pointer to an array of DST letters (e.g. "D", "S"). */
  const char* const* const letters;

  /** Number of rules in array. */
  uint8_t const num_rules;

  /** Number of letters in array. */
  uint8_t const num_letters;
};

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
struct AtcZoneContext {
  /*
   * Epoch year. Currently always 2000 but could change in the future. We're
   * leaving this out for now because it's not clear how or if the various
   * AceTime classes can use this information since the value '2000' is often
   * a compile-time constant instead of a runtime constant.
   */
  //int16_t epoch_year;

  /** Start year of the zone files. */
  int16_t start_year;

  /** Until year of the zone files. */
  int16_t until_year;

  /** TZ Database version which generated the zone info. */
  const char *tz_version;

  /** Number of fragments. */
  uint8_t num_fragments;

  /** Zone Name fragment list. */
  const char * const *fragments;
};

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
struct AtcZoneEra {
  /**
   * Zone policy, determined by the RULES column. Set to nullptr if the RULES
   * column is '-' or an explicit DST shift in the form of 'hh:mm'.
   */
  const struct AtcZonePolicy * const zone_policy;

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
   * entry from the TZ dtabase. There is an internal object named
   * `ExtendedZoneProcessor::kAnchorEra` which does set this field to nullptr.
   * Maybe it should be set to ""?
   */
  const char * const format;

  /** UTC offset in 15 min increments. Determined by the STDOFF column. */
  int8_t const offset_code;

  /**
   * If zone_policy is nullptr, then this indicates the DST offset in 15 minute
   * increments as defined by the RULES column in 'hh:mm' format. If the
   * 'RULES' column is '-', then the delta_code is 0.
   *
   * If the '--scope extended' flag is given to tzcompiler.py, the 'delta_code`
   * should be interpreted as a uint8_t field, composed of two 4-bit fields:
   *
   *    * The upper 4-bits is an unsigned integer from 0 to 14 that represents
   *    the one-minute remainder from the offset_code. This allows us to capture
   *    STDOFF offsets in 1-minute resolution.
   *    * The lower 4-bits is an unsigned integer that holds (originalDeltaCode
   *    + 4). This allows us to represent DST offsets from -1:00 to +2:45, in
   *    15-minute increments.
   *
   * The extended::ZoneEraBroker::deltaMinutes() and offsetMinutes() know how
   * to convert offset_code and delta_code into the appropriate minutes.
   */
  int8_t const delta_code;

  /**
   * Era is valid until currentTime < untilYear. Stored as (year - 2000) in a
   * single byte to save space. Comes from the UNTIL column.
   */
  int8_t const until_year_tiny;

  /** The month field in UNTIL (1-12). Will never be 0. */
  uint8_t const until_month;

  /**
   * The day field in UNTIL (1-31). Will never be 0. Also, there's no need for
   * untilDayOfWeek, because the database generator will resolve the exact day
   * of month based on the known year and month.
   */
  uint8_t const until_day;

  /**
   * The time field of UNTIL field in 15-minute increments. A range of 00:00 to
   * 25:00 corresponds to 0-100.
   */
  uint8_t const until_time_code;

  /**
   * The until_time_modifier is a packed field containing 2 pieces of info:
   *
   *    * The upper 4 bits represent the UNTIL time suffix: 'w', 's' or 'u',
   *    represented by kAtcSuffixW, kAtcSuffixS and kAtcSuffixU.
   *    * The lower 4 bits represent the remaining 0-14 minutes of the UNTIL
   *    field after truncation into untilTimeCode. In other words, the full
   *    UNTIL field in one-minute resolution is (15 * untilTimeCode +
   *    (until_time_modifier & 0x0f)).
   */
  uint8_t const until_time_modifier;
};

/**
 * Representation of a given time zone, implemented as an array of ZoneEra
 * records.
 */
struct AtcZoneInfo {
  /** Full name of zone (e.g. "America/Los_Angeles"). */
  const char * const name;

  /**
   * Unique, stable ID of the zone name, created from a hash of the name.
   * This ID will never change once assigned. This can be used for presistence
   * and serialization.
   */
  uint32_t const zone_id;

  /** ZoneContext metadata. */
  const struct AtcZoneContext * const zone_context;

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
  const void * const eras;
};

//---------------------------------------------------------------------------

/**
 * A LINK to ZONE mapping, using the zoneId/linkId hash key.
 */
struct AtcLinkEntry {
  /** Hash id of the source LINK name (e.g. "US/Pacific"). */
  uint32_t const link_id;

  /** Hash id of the target ZONE name (e.g. "America/Los_Angeles"). */
  uint32_t const zone_id;
};

#endif
