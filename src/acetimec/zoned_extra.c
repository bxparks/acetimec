/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include "epoch.h"
#include "time_zone.h"
#include "zoned_extra.h"

void atc_zoned_extra_set_error(AtcZonedExtra *extra)
{
  extra->fold_type = kAtcFoldTypeNotFound;
}

bool atc_zoned_extra_is_error(const AtcZonedExtra *extra)
{
  return extra->fold_type == kAtcFoldTypeNotFound;
}

void atc_zoned_extra_from_epoch_seconds(
    AtcZonedExtra *extra,
    atc_time_t epoch_seconds,
    const AtcTimeZone *tz)
{
  atc_time_zone_zoned_extra_from_epoch_seconds(tz, epoch_seconds, extra);
}

void atc_zoned_extra_from_unix_seconds(
    AtcZonedExtra *extra,
    int64_t unix_seconds,
    const AtcTimeZone *tz)
{
  atc_time_t epoch_seconds = atc_epoch_seconds_from_unix_seconds(unix_seconds);
  atc_time_zone_zoned_extra_from_epoch_seconds(tz, epoch_seconds, extra);
}

void atc_zoned_extra_from_local_date_time(
    AtcZonedExtra *extra,
    const AtcLocalDateTime *ldt,
    const AtcTimeZone *tz,
    uint8_t disambiguate) {
  atc_time_zone_zoned_extra_from_local_date_time(tz, ldt, disambiguate, extra);
}
