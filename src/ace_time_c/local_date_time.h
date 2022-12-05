/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file local_date_time.h
 *
 * Functions that relate to the local date time, without any reference to the
 * time zone.
 */

#ifndef ACE_TIME_C_LOCAL_DATE_TIME_H
#define ACE_TIME_C_LOCAL_DATE_TIME_H

#include <stdint.h>
#include "common.h"
#include "string_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Date and time fields which are independent of the time zone. Often this
 * struct holds the date time fields in UTC, but not always.
 */
typedef struct AtcLocalDateTime {
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
} AtcLocalDateTime;

/**
 * Convert LocalDateTime in UTC to epoch seconds.
 * Return kAtcInvalidEpochSeconds upon failure.
 */
atc_time_t atc_local_date_time_to_epoch_seconds(
    const AtcLocalDateTime *ldt);

/**
 * Convert epoch seconds to LocalDateTime in UTC.
 * Return non-zero error code upon failure.
 */
int8_t atc_local_date_time_from_epoch_seconds(
  atc_time_t epoch_seconds,
  AtcLocalDateTime *ldt);

/** Print the local date time in ISO 8601 format. */
void atc_local_date_time_print(
    AtcStringBuffer *sb,
    const AtcLocalDateTime *ldt);

#ifdef __cplusplus
}
#endif

#endif
