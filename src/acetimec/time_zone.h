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

#include "../zoneinfo/zone_info.h"
#include "zone_processor.h"

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct AtcOffsetDateTime AtcOffsetDateTime;
typedef struct AtcZonedExtra AtcZonedExtra;

/** A data structure that represents a specific Time Zone. */
typedef struct AtcTimeZone {
  /** Pointer to an AtcZoneInfo that contains the IANA TZ database info. */
  const AtcZoneInfo *zone_info;

  /**
   * Pointer to a pre-allocated preallocated AtcZoneProcessor which knows how
   * to calculate the DST transitions of the particular time zone.
   */
  AtcZoneProcessor *zone_processor;
} AtcTimeZone;

/**
 * Convert epoch_seconds to an AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_offset_date_time_from_epoch_seconds(
  const AtcTimeZone *tz,
  atc_time_t epoch_seconds,
  AtcOffsetDateTime *odt);

/**
 * Convert the LocalDateTime to AtcOffsetDateTime using the given zone_info.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_offset_date_time_from_local_date_time(
  const AtcTimeZone *tz,
  const AtcLocalDateTime *ldt,
  AtcOffsetDateTime *odt);

/**
 * Populate the ZonedExtra using the given epoch seconds.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_zoned_extra_from_epoch_seconds(
  const AtcTimeZone *tz,
  atc_time_t epoch_seconds,
  AtcZonedExtra *extra);

/**
 * Populate the ZonedExtra using the given local date time.
 * Return non-zero error code upon failure.
 */
int8_t atc_time_zone_zoned_extra_from_local_date_time(
  const AtcTimeZone *tz,
  const AtcLocalDateTime *ldt,
  AtcZonedExtra *extra);

#ifdef __cplusplus
}
#endif

#endif
