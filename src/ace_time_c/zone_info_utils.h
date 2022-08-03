/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_INFO_UTILS_H
#define ACE_TIME_C_ZONE_INFO_UTILS_H

#include <stdint.h>

/**
 * Convert time code (in 15 minute increments to minutes
 * The lower 4-bits of the modifier contains the remaining 0-14 minutes.
 */
uint16_t atc_zone_info_time_code_to_minutes(
  uint8_t code, uint8_t modifier);

/** Extract the time suffix (w, s, gu) from the upper 4-bits of the modifier. */
uint8_t atc_zone_info_modifier_to_suffix(uint8_t modifier);

#endif
