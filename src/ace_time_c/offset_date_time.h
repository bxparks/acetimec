/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file offset_date_time.h
 *
 * Functions that relate to the local date time along with a UTC offset measured
 * in minutes. For example, this can be used to represent a fixed offset from
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
 * Date and time fields with a UTC offset in minutes.
 *
 * In AceTime, the 'fold' parameter was included in the low-level LocalTime
 * class, which then got absorbed into the LocalDatetime class. This extra
 * parameter is mostly transparent to the user because C++ supports default
 * parameters in the constructor and functions.
 *
 * Unfortunately C does not have default parameters, so adding a 'fold' in
 * LocalDateTime causes unnecessary friction. Therefore, I am adding this
 * parameter to the OffsetDateTime instead, which is higher level so hopefully
 * the parameter is exposed to the user only when the user needs.
 */
typedef struct AtcOffsetDateTime {
  /** year [0,9999] */
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
  /** fold [0,1] */
  uint8_t fold;

  /** offset_minutes [-840, 960] */
  int16_t offset_minutes;
} AtcOffsetDateTime;

/** Return the epoch seconds for the given AtcOffsetDateTime. */
atc_time_t atc_offset_date_time_to_epoch_seconds(
    const AtcOffsetDateTime *odt);

/**
 * Create the AtcOffsetDateTime from the epoch_seconds and total offset minutes.
 * The 'fold' parameter is explicitly set to 0.
 * Returns non-zero error code upon failure.
 */
int8_t atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    AtcOffsetDateTime *odt);

/** Print the offset date time in ISO 8601 format. */
void atc_offset_date_time_print(
    const AtcOffsetDateTime *odt,
    AtcStringBuffer *sb);

#ifdef __cplusplus
}
#endif

#endif
