/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_LOCAL_DATE_TIME_H
#define ACE_TIME_C_LOCAL_DATE_TIME_H

#include <stdint.h>
#include <stdbool.h>
#include "common.h"

/**
 * Date and time fields which are independent of the time zone. Often this
 * struct holds the date time fields in UTC, but not always.
 */
struct AtcLocalDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

/** Convert LocalDateTime in UTC to epoch seconds. */
atc_time_t atc_local_date_time_to_epoch_seconds(
    const struct AtcLocalDateTime *ldt);

/** Convert epoch seconds to LocalDateTime in UTC. */
bool atc_local_date_time_from_epoch_seconds(
  atc_time_t epoch_seconds,
  struct AtcLocalDateTime *ldt);

#endif
