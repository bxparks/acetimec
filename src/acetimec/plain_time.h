/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file plain_time.h
 *
 * Low-level time functions.
 */

#ifndef ACE_TIME_C_PLAIN_TIME_H
#define ACE_TIME_C_PLAIN_TIME_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Validate the PlainTime (hour, minute, second) triple and return true if
 * valid.
 */
bool atc_plain_time_is_valid(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * Convert the given (hour, minute, second) to the number of seconds since
 * 00:00:00 of that day. Returns kAtcInvalidSeconds if the parameters are not
 * valid.
 */
int32_t atc_plain_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second);

#ifdef __cplusplus
}
#endif

#endif
