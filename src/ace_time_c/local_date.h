/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file local_date.h
 *
 * Low-level date functions, for example, for calculating leap years, day of
 * week, number of days in a specific month, and converting epoch seconds to
 * date-time components.
 *
 * Uses the algorithm described in
 * https://howardhinnant.github.io/date_algorithms.html.
 */

#ifndef ACE_TIME_C_LOCAL_DATE_H
#define ACE_TIME_C_LOCAL_DATE_H

#include <stdbool.h>
#include <stdint.h>

/** ISO Weekday numbers. Monday=1, Sunday=7. */
enum {
  kAtcIsoWeekdayMonday = 1,
  kAtcIsoWeekdayTuesday,
  kAtcIsoWeekdayWednesday,
  kAtcIsoWeekdayThursday,
  kAtcIsoWeekdayFriday,
  kAtcIsoWeekdaySaturday,
  kAtcIsoWeekdaySunday,
};

enum {
  /**
   * Base epoch year used by the lowest level epoch seconds converter.
   * This is an internal implementation detail and should not normally be needed
   * by client applications. They should instead use
   * atc_get_current_epoch_year() and atc_set_current_epoch_year().
   */
  kAtcBaseEpochYear = 2000,

  /** Sentinel value for invalid year. */
  kAtcInvalidYear = INT16_MIN,

  /** Invalid epoch seconds. */
  kAtcInvalidEpochSeconds = INT32_MIN,
};

/**
 * The epoch year which will be used to interpret the epoch seconds. By default,
 * the current epoch year is 2000, which means that the epoch is
 * 2000-01-01T00:00:00, and the largest date that can be represented by an
 * `int32_t` epoch_seconds is 2068-01-19T03:14:07. To represents dates after
 * this, we would have to change the current epoch year. For example, changing
 * the current epoch year to 2100 allows the epoch_seconds to extend to
 * 2168-01-20T03:14:07.
 */
extern int16_t atc_current_epoch_year;

/** Number of days from epoch converter base epoch to the current epoch. */
extern int32_t atc_days_from_base_epoch_to_current_epoch;

/** Get the current epoch year. */
int16_t atc_get_current_epoch_year();

/** Set the current epoch year. */
void atc_set_current_epoch_year(int16_t year);

/**
 * The smallest year (inclusive) for which calculations involving the 32-bit
 * `epoch_seconds` and time zone transitions are guaranteed to be valid without
 * underflowing or overflowing. Valid years satisfy the condition `year >=
 * atc_epoch_valid_year_lower()`.
 *
 * A 32-bit integer has a range of about 136 years, so the half interval is 68
 * years. But the algorithms to calculate transitions in `zone_processing.h` use
 * a 3-year window straddling the current year, so the actual lower limit is
 * probably closer to `atc_get_current_epoch_year() - 66`. To be conservative,
 * this function returns `atc_get_current_epoch_year() - 50`. It may return a
 * smaller value in the future if the internal calculations can be verified to
 * avoid underflow or overflow problems.
 */
int16_t atc_epoch_valid_year_lower();

/**
 * The largest year (exclusive) for which calculations involving the 32-bit
 * `epoch_seconds` and time zone transitions are guaranteed to be valid without
 * underflowing or overflowing. Valid years satisfy the condition `year <
 * atc_epoch_valid_year_upper()`.
 *
 * A 32-bit integer has a range of about 136 years, so the half interval is 68
 * years. But the algorithms to calculate the transitions in `zone_processing.h`
 * use a 3-year window straddling the current year, so actual upper limit is
 * probably close to `atc_get_current_epoch_year() + 66`. To be conservative,
 * this function returns `atc_get_current_epoch_year() + 50`. It may return a
 * larger value in the future if the internal calculations can be verified to
 * avoid underflow or overflow problems.
 */
int16_t atc_epoch_valid_year_upper();

/** Return true if year is a leap year. */
bool atc_is_leap_year(int16_t year);

/** Return the number of days in the month for the given year. */
uint8_t atc_local_date_days_in_year_month(int16_t year, uint8_t month);

/**
 * Calculate the ISO day of week (i.e. Monday=1, Sunday=7) given the (year,
 * month, day). Idea borrowed from https://github.com/evq/utz. No validation of
 * year, month or day is performed.
 */
uint8_t atc_local_date_day_of_week(int16_t year, uint8_t month, uint8_t day);

/**
 * Return the number of epoch days from the base epoch year (2000) for the
 * (year, month, day) triple.
 * This algorithm corresponds to
 * AceTime/src/ace_time/internal/EpochConverterHinnant.h.
 *
 * @param year [1,9999]
 * @param month month integer, [1,12]
 * @param day day of month integer, [1,31]
 */
int32_t atc_local_date_to_base_epoch_days(
    int16_t year, uint8_t month, uint8_t day);

/**
 * Return the number of days from the current epoch year for the (year, month,
 * day) triple.
 *
 * @param year [1,9999]
 * @param month month integer, [1,12]
 * @param day day of month integer, [1,31]
 */
int32_t atc_local_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day);

/**
 * Extract the (year, month, day) fields from the epoch days from the base epoch
 * year (2000).
 * This algorithm corresponds to
 * AceTime/src/ace_time/internal/EpochConverterHinnant.h.
 *
 * No input validation is performed. The behavior is undefined if the
 * parameters are outside their expected range.
 *
 * @param epoch_days number of days from base epoch of 2000-01-01
 * @param year year [1,9999]
 * @param month month integer [1, 12]
 * @param day day of month integer[1, 31]
 */
void atc_local_date_from_base_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day);

/**
 * Extract the (year, month, day) fields from the current epoch year
 * defined by atc_set_current_epoch_year().
 *
 * No input validation is performed. The behavior is undefined if the
 * parameters are outside their expected range.
 *
 * @param epoch_days number of days from the current epoch year
 * @param year year [1,9999]
 * @param month month integer [1, 12]
 * @param day day of month integer[1, 31]
 */
void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day);

/** Local Date struct. */
typedef struct AtcLocalDate {
  /** year [0,10000] */
  int16_t year;

  /** month [1,12] */
  uint8_t month;

  /** day [1,31] */
  uint8_t day;
} AtcLocalDate;

/** Increment given AtcLocalDate by one day. */
void atc_local_date_increment_one_day(AtcLocalDate *ld);

/** Decrement given AtcLocalDate by one day. */
void atc_local_date_decrement_one_day(AtcLocalDate *ld);

#endif
