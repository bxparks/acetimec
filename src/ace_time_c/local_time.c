/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_time.h"

int32_t atc_local_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second)
{
  return ((hour * (int16_t) 60) + minute) * (int32_t) 60 + second;
}

