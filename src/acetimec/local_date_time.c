/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date_time.h"

#include "common.h"
#include "epoch.h"
#include "local_date.h"
#include "local_time.h"
#include "local_date_time.h"

void atc_local_date_time_set_error(AtcLocalDateTime *ldt)
{
  ldt->month = 0; // year 0 is valid, so can't use year field
}

bool atc_local_date_time_is_error(const AtcLocalDateTime *ldt)
{
  return ldt->month == 0;
}

atc_time_t atc_local_date_time_to_epoch_seconds(
    const AtcLocalDateTime *ldt)
{
  if (atc_local_date_time_is_error(ldt)) return kAtcInvalidEpochSeconds;

  int32_t days = atc_local_date_to_epoch_days(
      ldt->year, ldt->month, ldt->day);
  int32_t seconds = atc_local_time_to_seconds(
      ldt->hour, ldt->minute, ldt->second);
  return days * 86400 + seconds;
}

void atc_local_date_time_from_epoch_seconds(
  AtcLocalDateTime *ldt,
  atc_time_t epoch_seconds)
{
  ldt->fold = 0;

  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    atc_local_date_time_set_error(ldt);
    return;
  }

  // Integer floor-division towards -infinity
  int32_t days = (epoch_seconds < 0)
      ? (epoch_seconds + 1) / 86400 - 1
      : epoch_seconds / 86400;
  int32_t seconds = epoch_seconds - 86400 * days;

  // Extract (year, month day).
  atc_local_date_from_epoch_days(days, &ldt->year, &ldt->month, &ldt->day);

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  ldt->second = seconds % 60;
  uint16_t minutes = seconds / 60;
  ldt->minute = minutes % 60;
  ldt->hour = minutes / 60;
}

int64_t atc_local_date_time_to_unix_seconds(const AtcLocalDateTime *ldt) {
  if (atc_local_date_time_is_error(ldt)) return kAtcInvalidUnixSeconds;

  int32_t days = atc_local_date_to_epoch_days(
      ldt->year, ldt->month, ldt->day);
  int32_t seconds = atc_local_time_to_seconds(
      ldt->hour, ldt->minute, ldt->second);
  int32_t unix_days = atc_unix_days_from_epoch_days(days);
  return unix_days * (int64_t)86400 + seconds;
}

void atc_local_date_time_from_unix_seconds(
  AtcLocalDateTime *ldt,
  int64_t unix_seconds)
{
  ldt->fold = 0;

  if (unix_seconds == kAtcInvalidUnixSeconds) {
    atc_local_date_time_set_error(ldt);
    return;
  }

  // Integer floor-division towards -infinity. Implicitly assume that
  // unix_seconds/86400 fits inside an int32_t. So the largest/smallest days is
  // about +/- 2^31, which translates to +/- 5_879_489 years, which I think is
  // more than sufficient for the foreseeable future.
  int32_t unix_days = (unix_seconds < 0)
      ? (unix_seconds + 1) / 86400 - 1
      : unix_seconds / 86400;
  int32_t seconds = unix_seconds - 86400 * unix_days;

  int32_t days = atc_epoch_days_from_unix_days(unix_days);

  // Extract (year, month day).
  atc_local_date_from_epoch_days(days, &ldt->year, &ldt->month, &ldt->day);

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  ldt->second = seconds % 60;
  uint16_t minutes = seconds / 60;
  ldt->minute = minutes % 60;
  ldt->hour = minutes / 60;
}

void atc_local_date_time_print(
    AtcStringBuffer *sb,
    const AtcLocalDateTime *ldt)
{
  atc_print_uint16_pad4(sb, ldt->year);
  atc_print_char(sb, '-');
  atc_print_uint16_pad2(sb, ldt->month);
  atc_print_char(sb, '-');
  atc_print_uint16_pad2(sb, ldt->day);
  atc_print_char(sb, 'T');
  atc_print_uint16_pad2(sb, ldt->hour);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, ldt->minute);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, ldt->second);
}
