/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdint.h>
#include "epoch.h"
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

int32_t atc_local_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day)
{
  int32_t converter_days = atc_convert_to_days(year, month, day);
  return converter_days - atc_get_days_to_current_epoch_from_converter_epoch();
}

void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day)
{
  int32_t converter_days = epoch_days
      + atc_get_days_to_current_epoch_from_converter_epoch();
  atc_convert_from_days(converter_days, year, month, day);
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
