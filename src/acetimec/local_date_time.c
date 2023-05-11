/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date_time.h"

#include "common.h"
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
  if (ldt->year == kAtcInvalidYear) return kAtcInvalidEpochSeconds;

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

  return;
}

void atc_local_date_time_print(
    const AtcLocalDateTime *ldt,
    AtcStringBuffer *sb)
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
