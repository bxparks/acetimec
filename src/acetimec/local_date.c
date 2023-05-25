/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdint.h>
#include "epoch.h"
#include "local_date.h"

// Offsets used to calculate the day of the week of a particular (year, month,
// day). The element represents the number of days that the first of month of
// the given index was shifted by the cummulative days from the previous months.
// To determine the "day of the week", we must normalize the resulting "day of
// the week" modulo 7.
//
// January is index 0, but we also use a modified year, where the year starts in
// March to make leap years easier to handle, so the shift for March=3 is 0.
//
// For example:
//    * atc_days_of_week[3] is 3 because April (index=3) 1st is shifted by 3
//      days because March has 31 days (28 + 3).
//    * atc_days_of_week[4] is 5 because May (index=4) 1st is shifted by 2
//      additional days from April, because April has 30 days (28 + 2).
static const uint8_t atc_days_of_week[12] = {
  5 /*Jan=31*/,
  1 /*Feb=28*/,
  0 /*Mar=31, start of "year"*/,
  3 /*Apr=30*/,
  5 /*May=31*/,
  1 /*Jun=30*/,
  3 /*Jul=31*/,
  6 /*Aug=31*/,
  2 /*Sep=30*/,
  4 /*Oct=31*/,
  0 /*Nov=30*/,
  2 /*Dec=31*/,
};

static const uint8_t atc_days_in_month[12] = {
  31 /*Jan=31*/,
  28 /*Feb=28*/,
  31 /*Mar=31*/,
  30 /*Apr=30*/,
  31 /*May=31*/,
  30 /*Jun=30*/,
  31 /*Jul=31*/,
  31 /*Aug=31*/,
  30 /*Sep=30*/,
  31 /*Oct=31*/,
  30 /*Nov=30*/,
  31 /*Dec=31*/,
};

bool atc_is_leap_year(int16_t year)
{
  return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

uint8_t atc_local_date_days_in_year_month(int16_t year, uint8_t month)
{
  uint8_t days = atc_days_in_month[month - 1];
  return (month == 2 && atc_is_leap_year(year)) ? days + 1 : days;
}

uint8_t atc_local_date_day_of_week(int16_t year, uint8_t month, uint8_t day)
{
  // The "year" starts in March to shift leap year calculation to end.
  int16_t y = year - (month < 3);

  // Each year shifts the day of week by one. Each leap year by one.
  // Except every 100 years. Unless divisible by 400.
  int16_t d = y + y/4 - y/100 + y/400 + atc_days_of_week[month-1] + day;

  // 2000-01-01 was a Saturday=6, so set the offsets accordingly
  return (d < -1) ? (d + 1) % 7 + 8 : (d + 1) % 7 + 1;
}

int32_t atc_local_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day)
{
  int32_t internal_days = atc_convert_to_internal_days(year, month, day);
  return internal_days - atc_get_days_to_current_epoch_from_internal_epoch();
}

void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day)
{
  int32_t internal_days = epoch_days
      + atc_get_days_to_current_epoch_from_internal_epoch();
  atc_convert_from_internal_days(internal_days, year, month, day);
}

void atc_local_date_increment_one_day(
    int16_t *year, uint8_t *month, uint8_t *day)
{
  (*day)++;

  if (*day > atc_local_date_days_in_year_month(*year, *month)) {
    *day = 1;
    (*month)++;
    if (*month > 12) {
      *month = 1;
      (*year)++;
    }
  }
}

void atc_local_date_decrement_one_day(
    int16_t *year, uint8_t *month, uint8_t *day)
{
  (*day)--;

  if (*day == 0) {
    (*month)--;
    if (*month == 0) {
      *month = 12;
      (*year)--;
      *day = 31;
    } else {
      *day = atc_local_date_days_in_year_month(*year, *month);
    }
  }
}
