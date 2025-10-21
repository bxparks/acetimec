/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file plain_date_time.h
 *
 * Functions that relate to the local date time, without any reference to the
 * time zone.
 */

#ifndef ACE_TIME_C_PLAIN_DATE_TIME_H
#define ACE_TIME_C_PLAIN_DATE_TIME_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "string_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Date and time fields which are independent of the time zone. Often this
 * struct holds the date time fields in UTC, but not always.
 */
typedef struct AtcPlainDateTime {
  /** year [0,9999] */
  int16_t year;
  /** month [1,12]; 0 indicates error value */
  uint8_t month;
  /** day [1,31] */
  uint8_t day;
  /** hour [0-23] */
  uint8_t hour;
  /** minute [0, 59] */
  uint8_t minute;
  /** second [0, 59] */
  uint8_t second;
} AtcPlainDateTime;

/** Set the given AtcPlainDateTime to its error state. */
void atc_plain_date_time_set_error(AtcPlainDateTime *pdt);

/** Return true if AtcPlainDateTime is in an error state. */
bool atc_plain_date_time_is_error(const AtcPlainDateTime *pdt);

/** Return true if AtcPlainDateTime is valid. */
bool atc_plain_date_time_is_valid(const AtcPlainDateTime *pdt);

/**
 * Convert PlainDateTime in UTC to epoch seconds.
 * Return kAtcInvalidEpochSeconds if `pdt` is not validate.
 */
atc_time_t atc_plain_date_time_to_epoch_seconds(
    const AtcPlainDateTime *pdt);

/**
 * Convert epoch seconds to PlainDateTime in UTC.
 * Return an error value for `pdt` upon error.
 */
void atc_plain_date_time_from_epoch_seconds(
  AtcPlainDateTime *pdt,
  atc_time_t epoch_seconds);

/**
 * Convert PlainDateTime in UTC to Unix seconds (since 1970).
 * Return kAtcInvalidUnixSeconds if `pdt` is not validate.
 */
int64_t atc_plain_date_time_to_unix_seconds(
    const AtcPlainDateTime *pdt);

/**
 * Convert epoch seconds to PlainDateTime in UTC.
 * Return an error value for pdt upon error.
 */
void atc_plain_date_time_from_unix_seconds(
  AtcPlainDateTime *pdt,
  int64_t unix_seconds);

/** Print the local date time in ISO 8601 format. */
void atc_plain_date_time_print(
    AtcStringBuffer *sb,
    const AtcPlainDateTime *pdt);

#ifdef __cplusplus
}
#endif

#endif
