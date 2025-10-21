/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "plain_date_time.h"

#include "common.h"
#include "epoch.h"
#include "plain_date.h"
#include "plain_time.h"
#include "plain_date_time.h"

void atc_plain_date_time_set_error(AtcPlainDateTime *pdt)
{
  pdt->month = 0; // year 0 is valid, so can't use year field
}

bool atc_plain_date_time_is_error(const AtcPlainDateTime *pdt)
{
  return pdt->month == 0;
}

bool atc_plain_date_time_is_valid(const AtcPlainDateTime *pdt)
{
  if (! atc_plain_date_is_valid(pdt->year, pdt->month, pdt->day)) {
    return false;
  }
  if (! atc_plain_time_is_valid(pdt->hour, pdt->minute, pdt->second)) {
    return false;
  }
  return true;
}

atc_time_t atc_plain_date_time_to_epoch_seconds(
    const AtcPlainDateTime *pdt)
{
  if (atc_plain_date_time_is_error(pdt)) return kAtcInvalidEpochSeconds;

  int32_t days = atc_plain_date_to_epoch_days(
      pdt->year, pdt->month, pdt->day);
  if (days == kAtcInvalidEpochDays) return kAtcInvalidEpochSeconds;

  int32_t seconds = atc_plain_time_to_seconds(
      pdt->hour, pdt->minute, pdt->second);
  if (seconds == kAtcInvalidEpochSeconds) return kAtcInvalidEpochSeconds;

  return days * 86400 + seconds;
}

void atc_plain_date_time_from_epoch_seconds(
  AtcPlainDateTime *pdt,
  atc_time_t epoch_seconds)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    atc_plain_date_time_set_error(pdt);
    return;
  }

  // Integer floor-division towards -infinity
  int32_t days = (epoch_seconds < 0)
      ? (epoch_seconds + 1) / 86400 - 1
      : epoch_seconds / 86400;
  int32_t seconds = epoch_seconds - 86400 * days;

  // Extract (year, month day).
  atc_plain_date_from_epoch_days(days, &pdt->year, &pdt->month, &pdt->day);

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  pdt->second = seconds % 60;
  uint16_t minutes = seconds / 60;
  pdt->minute = minutes % 60;
  pdt->hour = minutes / 60;
}

int64_t atc_plain_date_time_to_unix_seconds(const AtcPlainDateTime *pdt) {
  if (atc_plain_date_time_is_error(pdt)) return kAtcInvalidUnixSeconds;

  int32_t days = atc_plain_date_to_epoch_days(
      pdt->year, pdt->month, pdt->day);
  if (days == kAtcInvalidUnixDays) return kAtcInvalidUnixSeconds;

  int32_t seconds = atc_plain_time_to_seconds(
      pdt->hour, pdt->minute, pdt->second);
  if (seconds == kAtcInvalidSeconds) return kAtcInvalidUnixSeconds;

  int32_t unix_days = atc_unix_days_from_epoch_days(days);
  return unix_days * (int64_t)86400 + seconds;
}

void atc_plain_date_time_from_unix_seconds(
  AtcPlainDateTime *pdt,
  int64_t unix_seconds)
{
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    atc_plain_date_time_set_error(pdt);
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
  atc_plain_date_from_epoch_days(days, &pdt->year, &pdt->month, &pdt->day);

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  pdt->second = seconds % 60;
  uint16_t minutes = seconds / 60;
  pdt->minute = minutes % 60;
  pdt->hour = minutes / 60;
}

void atc_plain_date_time_print(
    AtcStringBuffer *sb,
    const AtcPlainDateTime *pdt)
{
  atc_print_uint16_pad4(sb, pdt->year);
  atc_print_char(sb, '-');
  atc_print_uint16_pad2(sb, pdt->month);
  atc_print_char(sb, '-');
  atc_print_uint16_pad2(sb, pdt->day);
  atc_print_char(sb, 'T');
  atc_print_uint16_pad2(sb, pdt->hour);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, pdt->minute);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, pdt->second);
}
