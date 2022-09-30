/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdint.h>
#include "local_date.h"

// Using 0=Jan offset.
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

int16_t atc_local_epoch_year = 2000;

int32_t atc_days_to_local_epoch_from_base_epoch = 0;

int16_t atc_get_local_epoch_year()
{
  return atc_local_epoch_year;
}

void atc_set_local_epoch_year(int16_t year)
{
  atc_local_epoch_year = year;
  atc_days_to_local_epoch_from_base_epoch = atc_local_date_to_base_epoch_days(
      year, 1, 1);
}

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
  int16_t d = y + y/4 - y/100 + y/400 + atc_days_of_week[month-1] + day;

  // 2000-01-01 was a Saturday=6, so set the offsets accordingly
  return (d < -1) ? (d + 1) % 7 + 8 : (d + 1) % 7 + 1;
}

/** Return the number days before the given month_prime. */
static uint16_t atc_to_days_until_month_prime(uint8_t month_prime)
{
  return (153 * month_prime + 2) / 5;
}

int32_t atc_local_date_to_base_epoch_days(
    int16_t year, uint8_t month, uint8_t day)
{
  int16_t year_prime = year - ((month <= 2) ? 1 : 0); // year begins on Mar 1
  uint16_t era = year_prime / 400; // [0,24]
  uint16_t year_of_era = year_prime - 400 * era; // [0,399]

  uint8_t month_prime = (month <= 2) ? month + 9 : month - 3; // [0,11]
  uint16_t days_until_month_prime = atc_to_days_until_month_prime(month_prime);
  uint16_t day_of_year_prime = days_until_month_prime + day - 1; // [0,365]
  uint32_t day_of_era = (uint32_t) 365 * year_of_era + (year_of_era / 4)
      - (year_of_era / 100) + day_of_year_prime; // [0,146096]

  int32_t day_of_epoch_prime = day_of_era + 146097 * era;
  return day_of_epoch_prime 
          - (2000 / 400) * 146097 /*shift relative to 2000-03-01*/
          + 60 /*shift relative to 2000-01-01, 2000 is a leap year*/;
}

int32_t atc_local_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day)
{
  int32_t days = atc_local_date_to_base_epoch_days(year, month, day);
}

void atc_local_date_from_base_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day)
{
  int32_t day_of_epoch_prime = epoch_days + (2000 / 400) * 146097 - 60;
  uint16_t era = (uint32_t) day_of_epoch_prime / 146097; // [0,24]
  uint32_t day_of_era = day_of_epoch_prime - 146097 * era; // [0,146096]
  uint16_t year_of_era = (day_of_era - day_of_era / 1460 + day_of_era / 36524
      - day_of_era / 146096) / 365; // [0,399]
  uint16_t year_prime = year_of_era + 400 * era; // [0,9999]
  uint16_t day_of_year_prime = day_of_era
      - (365 * year_of_era + year_of_era / 4 - year_of_era/100); // [0,365]
  uint8_t month_prime = (5 * day_of_year_prime + 2) / 153;
  uint16_t days_until_month_prime = atc_to_days_until_month_prime(month_prime);

  *day = day_of_year_prime - days_until_month_prime + 1; // [1,31]
  *month = (month_prime < 10) ? month_prime + 3 : month_prime - 9; // [1,12]
  *year = year_prime + ((*month <= 2) ? 1 : 0); // [1,9999]
}

void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day)
{
  int32_t days = epoch_days + atc_days_to_local_epoch_from_base_epoch;
  atc_local_date_from_base_epoch_days(days, year, month, day);
}

void atc_local_date_increment_one_day(AtcLocalDate *ld)
{
  uint8_t day = ld->day + 1;
  uint8_t month = ld->month;
  int16_t year = ld->year;

  if (day > atc_local_date_days_in_year_month(year, month)) {
    day = 1;
    month++;
    if (month > 12) {
      month = 1;
      year++;
    }
  }
  ld->day = day;
  ld->month = month;
  ld->year = year;
}

void atc_local_date_decrement_one_day(AtcLocalDate *ld)
{
  uint8_t day = ld->day - 1;
  uint8_t month = ld->month;
  int16_t year = ld->year;

  if (day == 0) {
    month--;
    if (month == 0) {
      month = 12;
      year--;
    }
    day = atc_local_date_days_in_year_month(year, month);
  }
  ld->day = day;
  ld->month = month;
  ld->year = year;
}
