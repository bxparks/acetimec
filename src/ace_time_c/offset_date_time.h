/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_OFFSET_DATE_TIME_H
#define ACE_TIME_C_OFFSET_DATE_TIME_H

#include <stdint.h>
#include "common.h" // atc_time_t

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
struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int16_t offset_minutes;
};

/** Return the epoch seconds for the given AtcOffsetDateTime. */
atc_time_t atc_offset_date_time_to_epoch_seconds(
    const struct AtcOffsetDateTime *odt);

/**
 * Create the AtcOffsetDateTime from the epoch_seconds and total offset minutes.
 * The 'fold' parameter is explicitly set to 0.
 * Returns non-zero error code upon failure.
 */
int8_t atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    struct AtcOffsetDateTime *odt);

#endif
