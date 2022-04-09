/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_LOCAL_DATE_TIME_H
#define ACE_TIME_C_LOCAL_DATE_TIME_H

#include <stdint.h>

/** Convert (year, month, day, hour, minute, second) in UTC to epoch seconds. */
int32_t atc_to_epoch_seconds(
  int16_t year, uint8_t month, uint8_t day,
  uint8_t hour, uint8_t minute, uint8_t second);

/** Convert epoch seconds to (year, month, day, hour, minute, second) in UTC. */
void atc_from_epoch_seconds(
  int32_t epoch_seconds,
  int16_t *year, uint8_t *month, uint8_t *day,
  uint8_t *hour, uint8_t *minute, uint8_t *second);


#endif
