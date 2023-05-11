/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "local_date.h"
#include "local_date_time.h"
#include "offset_date_time.h"

atc_time_t atc_offset_date_time_to_epoch_seconds(
    const AtcOffsetDateTime *odt)
{
  const AtcLocalDateTime *ldt = (const AtcLocalDateTime *) odt;
  atc_time_t es = atc_local_date_time_to_epoch_seconds(ldt);
  if (es == kAtcInvalidEpochSeconds) return es;
  return es - odt->offset_seconds;
}

int8_t atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int32_t offset_seconds,
    AtcOffsetDateTime *odt)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  epoch_seconds += offset_seconds;
  AtcLocalDateTime *ldt = (AtcLocalDateTime *) odt;
  atc_local_date_time_from_epoch_seconds(ldt, epoch_seconds);
  if (atc_local_date_time_is_error(ldt)) return kAtcErrGeneric;

  odt->offset_seconds = offset_seconds;
  return kAtcErrOk;
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
    const AtcOffsetDateTime *odt,
    AtcStringBuffer *sb)
{
  atc_local_date_time_print((const AtcLocalDateTime *) odt, sb);
  print_offset_seconds(sb, odt->offset_seconds);
}
