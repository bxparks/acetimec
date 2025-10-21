/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "common.h"
#include "plain_time.h"

bool atc_plain_time_is_valid(uint8_t hour, uint8_t minute, uint8_t second)
{
  if (hour >= 24) return false;
  if (minute >= 60) return false;
  if (second >= 60) return false;
  return true;
}

int32_t atc_plain_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second)
{
  if (! atc_plain_time_is_valid(hour, minute, second)) {
    return kAtcInvalidSeconds;
  }
  return ((hour * (int16_t) 60) + minute) * (int32_t) 60 + second;
}

