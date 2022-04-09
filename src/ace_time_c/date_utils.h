/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_DATE_UTILS_H
#define ACE_TIME_C_DATE_UTILS_H

#include <stdbool.h>
#include <stdint.h>

/** Return true if year is a leap year. */
bool atc_is_leap_year(uint16_t year);

/** Return the number of days in the month for the given year. */
uint8_t atc_days_in_year_month(int16_t year, uint8_t month);

#endif
