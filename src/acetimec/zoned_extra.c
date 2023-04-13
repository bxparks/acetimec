/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "time_zone.h"
#include "zoned_extra.h"

int8_t atc_zoned_extra_from_epoch_seconds(
    AtcZonedExtra *extra,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz)
{
  return atc_time_zone_zoned_extra_from_epoch_seconds(
      tz, epoch_seconds, extra);
}

int8_t atc_zoned_extra_from_local_date_time(
    AtcZonedExtra *extra,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz)
{
  return atc_time_zone_zoned_extra_from_local_date_time(tz, ldt, extra);
}
