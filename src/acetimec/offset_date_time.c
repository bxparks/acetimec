/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date.h"
#include "local_date_time.h"
#include "offset_date_time.h"

void atc_offset_date_time_set_error(AtcOffsetDateTime *odt)
{
  odt->month = 0; // year 0 is valid, so can't use year field
}

bool atc_offset_date_time_is_error(const AtcOffsetDateTime *odt)
{
  return odt->month == 0;
}

atc_time_t atc_offset_date_time_to_epoch_seconds(const AtcOffsetDateTime *odt) {
  const AtcLocalDateTime *ldt = (const AtcLocalDateTime *) odt;
  atc_time_t es = atc_local_date_time_to_epoch_seconds(ldt);
  if (es == kAtcInvalidEpochSeconds) return kAtcInvalidEpochSeconds;
  return es - odt->offset_seconds;
}

void atc_offset_date_time_from_epoch_seconds(
    AtcOffsetDateTime *odt,
    atc_time_t epoch_seconds,
    int32_t offset_seconds)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    atc_offset_date_time_set_error(odt);
    return;
  }

  epoch_seconds += offset_seconds;
  AtcLocalDateTime *ldt = (AtcLocalDateTime *) odt;
  atc_local_date_time_from_epoch_seconds(ldt, epoch_seconds);
  if (atc_local_date_time_is_error(ldt)) return;

  odt->offset_seconds = offset_seconds;
}

int64_t atc_offset_date_time_to_unix_seconds(const AtcOffsetDateTime *odt) {
  const AtcLocalDateTime *ldt = (const AtcLocalDateTime *) odt;
  int64_t unix_seconds = atc_local_date_time_to_unix_seconds(ldt);
  if (unix_seconds == kAtcInvalidUnixSeconds) return kAtcInvalidUnixSeconds;
  return unix_seconds - odt->offset_seconds;
}

void atc_offset_date_time_from_unix_seconds(
    AtcOffsetDateTime *odt,
    int64_t unix_seconds,
    int32_t offset_seconds)
{
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    atc_offset_date_time_set_error(odt);
    return;
  }

  unix_seconds += offset_seconds;
  AtcLocalDateTime *ldt = (AtcLocalDateTime *) odt;
  atc_local_date_time_from_unix_seconds(ldt, unix_seconds);
  if (atc_local_date_time_is_error(ldt)) return;

  odt->offset_seconds = offset_seconds;
}

// Print +/-hh:mm, ignoring any ss component.
static void print_offset_seconds(AtcStringBuffer *sb, int32_t seconds)
{
  if (seconds < 0) {
    atc_print_char(sb, '-');
    seconds = -seconds;
  } else {
    atc_print_char(sb, '+');
  }

  //uint16_t ss = seconds % 60; // ignore
  uint16_t minutes = seconds / 60;
  uint16_t mm = minutes % 60;
  uint16_t hh = minutes / 60;

  atc_print_uint16_pad2(sb, hh);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, mm);
}

void atc_offset_date_time_print(
    AtcStringBuffer *sb,
    const AtcOffsetDateTime *odt)
{
  atc_local_date_time_print(sb, (const AtcLocalDateTime *) odt);
  print_offset_seconds(sb, odt->offset_seconds);
}
