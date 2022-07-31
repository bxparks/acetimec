/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_LOCAL_DATE_H
#define ACE_TIME_C_LOCAL_DATE_H

#include <stdbool.h>
#include <stdint.h>

/** Return true if year is a leap year. */
bool atc_is_leap_year(uint16_t year);

/** Return the number of days in the month for the given year. */
uint8_t atc_local_date_days_in_year_month(int16_t year, uint8_t month);

/**
 * Calculate the ISO day of week (i.e. Monday=1, Sunday=7) given the (year,
 * month, day). Idea borrowed from https://github.com/evq/utz. No validation of
 * year, month or day is performed.
 */
uint8_t atc_local_date_day_of_week(int16_t year, uint8_t month, uint8_t day);

/**
 * Return the number of epoch days for the (year_tiny, month, day) triple.
 */
int32_t atc_local_date_to_epoch_days(
    int8_t year_tiny, uint8_t month, uint8_t day);

/**
 * Extract the (year_tiny, month, day) fields from AceTime epoch_days.
 *
 * No input validation is performed. The behavior is undefined if the
 * parameters are outside their expected range.
 *
 * @param epoch_days number of days from AceTime Epoch of 2000-01-01
 * @param year_tiny year offset since 2000 [-127, 127], with -128 indicating
 *        an error condition
 * @param month month integer [1, 12]
 * @param day day of month integer[1, 31]
 */
void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int8_t *year_tiny,
    uint8_t *month,
    uint8_t *day);

#endif
