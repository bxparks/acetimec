/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_EPOCH_CONVERTER_HINNANT2_H
#define ACE_TIME_C_EPOCH_CONVERTER_HINNANT2_H

#include <stdint.h>

/**
 * Return the number days before the given month_prime.
 * This uses a division by 8 and a correction for month 1 (i.e. February).
 * The division by 8 will be converted into a bit-shift by the compiler,
 * which should be a lot faster on processors without hardware division.
 */
static uint16_t atc_to_days_until_month_prime(uint8_t month_prime)
{
  return (245 * month_prime + 2) / 8 + (month_prime == 1 ? 1 : 0);
}

int32_t atc_to_epoch_days(int8_t year_tiny, uint8_t month, uint8_t day)
{
  int8_t year_prime_tiny = year_tiny - ((month <= 2) ? 1 : 0);
  int8_t era_tiny = (year_prime_tiny < 0) ? -1 : 0;
  uint8_t month_prime = (month <= 2) ? month + 9 : month - 3;
  uint16_t days_until_month_prime = atc_to_days_until_month_prime(month_prime);
  uint16_t day_of_year_prime = days_until_month_prime + day - 1;
  uint16_t year_of_era = year_prime_tiny + ((era_tiny < 0) ? 400 : 0);
  uint32_t day_of_era = (uint32_t) 365 * year_of_era + (year_of_era / 4)
      - (year_of_era / 100) + day_of_year_prime;

  int32_t day_of_ace_time_epoch_prime =
      day_of_era - ((era_tiny < 0) ? 146097 : 0);
  return day_of_ace_time_epoch_prime + 60;
}

void atc_from_epoch_days(
    int32_t epoch_days,
    int8_t *year_tiny,
    uint8_t *month,
    uint8_t *day)
{

  int32_t day_of_ace_time_epoch_prime = epoch_days - 60;
  int8_t era_tiny = (day_of_ace_time_epoch_prime < 0) ? -1 : 0;
  uint32_t day_of_era = day_of_ace_time_epoch_prime
      + ((era_tiny < 0) ? 146097 : 0);
  uint16_t year_of_era = (day_of_era - day_of_era / 1460 + day_of_era / 36524
      - ((day_of_era == 146096) ? 1 : 0)) / 365;
  int8_t year_prime_tiny = year_of_era - ((era_tiny < 0) ? 400 : 0);
  uint16_t day_of_year_prime = day_of_era
      - (365 * year_of_era + year_of_era / 4 - year_of_era/100);
  uint8_t month_prime = (5 * day_of_year_prime + 2) / 153;
  uint16_t days_until_month_prime = atc_to_days_until_month_prime(month_prime);

  *day = day_of_year_prime - days_until_month_prime + 1;
  *month = (month_prime < 10) ? month_prime + 3 : month_prime - 9;
  *year_tiny = year_prime_tiny + ((*month <= 2) ? 1 : 0);
}

#endif
