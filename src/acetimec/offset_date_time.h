/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file offset_date_time.h
 *
 * Functions that relate to the local date time along with a UTC offset measured
 * in seconds. For example, this can be used to represent a fixed offset from
 * UTC, such as "2022-08-30 14:45:00-08:00".
 */

#ifndef ACE_TIME_C_OFFSET_DATE_TIME_H
#define ACE_TIME_C_OFFSET_DATE_TIME_H

#include <stdint.h>
#include "common.h" // atc_time_t
#include "string_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Date and time fields with a UTC offset in seconds.
 *
 * In the AceTime library, the 'resolved' parameter was included in the
 * low-level PlainTime class, which then got absorbed into the PlainDatetime
 * class. This extra parameter is mostly transparent to the user because C++
 * supports default parameters in the constructor and functions.
 *
 * Unfortunately C does not support default parameters, so adding a 'resolved'
 * in PlainDateTime causes unnecessary friction. Therefore, I am adding this
 * parameter to the OffsetDateTime instead, which is higher level so hopefully
 * the parameter is exposed to the user only when the user needs it.
 */
typedef struct AtcOffsetDateTime {
  /** year [1,9999] */
  int16_t year;
  /** month [1,12] */
  uint8_t month;
  /** day [1,31] */
  uint8_t day;

  /** hour [0-23] */
  uint8_t hour;
  /** minute [0, 59] */
  uint8_t minute;
  /** second [0, 59] */
  uint8_t second;

  /** See 'resolved' in AtcZonedDateTime. */
  uint8_t resolved;

  /** offset_seconds [-10h,+14h] roughly */
  int32_t offset_seconds;
} AtcOffsetDateTime;

/** Set the given AtcOffsetDateTime to its error state. */
void atc_offset_date_time_set_error(AtcOffsetDateTime *odt);

/** Return true if AtcOffsetDateTime is an error. */
bool atc_offset_date_time_is_error(const AtcOffsetDateTime *odt);

/** Return the epoch seconds for the given AtcOffsetDateTime. Returns
 * kAtcInvalidEpochSeconds if `odt` is invalid.
 */
atc_time_t atc_offset_date_time_to_epoch_seconds(const AtcOffsetDateTime *odt);

/**
 * Create the AtcOffsetDateTime from the epoch_seconds and total offset seconds.
 * Return an error value for odt upon error.
 */
void atc_offset_date_time_from_epoch_seconds(
    AtcOffsetDateTime *odt,
    atc_time_t epoch_seconds,
    int32_t offset_seconds);

/** Return the Unix seconds for the given AtcOffsetDateTime. Returns
 * kAtcInvalidUnixSeconds if `odt` is invalid.
 */
int64_t atc_offset_date_time_to_unix_seconds(const AtcOffsetDateTime *odt);

/**
 * Create the AtcOffsetDateTime from the Unix seconds and total offset seconds.
 * Return an error value for odt upon error.
 */
void atc_offset_date_time_from_unix_seconds(
    AtcOffsetDateTime *odt,
    int64_t unix_seconds,
    int32_t offset_seconds);

/** Print the offset date time in ISO 8601 format. */
void atc_offset_date_time_print(
    AtcStringBuffer *sb,
    const AtcOffsetDateTime *odt);

#ifdef __cplusplus
}
#endif

#endif
