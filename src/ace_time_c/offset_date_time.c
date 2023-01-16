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
  return es - odt->offset_minutes * 60;
}

int8_t atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    AtcOffsetDateTime *odt)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  epoch_seconds += offset_minutes * 60;
  int8_t err = atc_local_date_time_from_epoch_seconds(
      (AtcLocalDateTime *) odt, epoch_seconds);
  if (err) return err;

  odt->fold = 0;
  odt->offset_minutes = offset_minutes;
  return kAtcErrOk;
}

static void print_offset_minutes(AtcStringBuffer *sb, uint16_t offset_minutes)
{
  uint16_t hours = offset_minutes / 60;
  uint16_t minutes = offset_minutes % 60;
  atc_print_uint16_pad2(sb, hours);
  atc_print_char(sb, ':');
  atc_print_uint16_pad2(sb, minutes);
}

void atc_offset_date_time_print(
    const AtcOffsetDateTime *odt,
    AtcStringBuffer *sb)
{
  atc_local_date_time_print((const AtcLocalDateTime *) odt, sb);
  if (odt->offset_minutes < 0) {
    atc_print_char(sb, '-');
    print_offset_minutes(sb, -odt->offset_minutes);
  } else {
    atc_print_char(sb, '+');
    print_offset_minutes(sb, odt->offset_minutes);
  }
}
