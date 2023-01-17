/*
 * MIT License
 * Copyright (c) 2023 Brian T. Park
 */

/**
 * @file time_zone.h
 *
 * A class representing a specific IANA time zone, containing a pointer to a
 * ZoneInfo data containing the TZ database info, and a ZoneProcessor instance
 * which calculates DST offsets from the ZoneInfo.
 */

#ifndef ACE_TIME_C_TIME_ZONE_H
#define ACE_TIME_C_TIME_ZONE_H

#include "zone_info.h"
#include "zone_processor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AtcOffsetDateTime AtcOffsetDateTime;

typedef struct AtcTimeZone {
  const AtcZoneInfo *zone_info;
  AtcZoneProcessor *zone_processor;
} AtcTimeZone;

/**
 * Convert epoch_seconds to an AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_offset_date_time_from_epoch_seconds(
  AtcTimeZone tz,
  atc_time_t epoch_seconds,
  AtcOffsetDateTime *odt);

/**
 * Convert the LocalDateTime to AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_offset_date_time_from_local_date_time(
  AtcTimeZone tz,
  const AtcLocalDateTime *ldt,
  AtcOffsetDateTime *odt);

#ifdef __cplusplus
}
#endif

#endif
