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

/** Forward declaration for AtcOffsetDateTime. */
typedef struct AtcOffsetDateTime AtcOffsetDateTime;

/** Forward declaration for AtcZonedExtra. */
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

/** A default time zone instance representing UTC. */
extern const AtcTimeZone atc_time_zone_utc;

/**
 * Convert epoch_seconds to an AtcOffsetDateTime using the given time zone.
 * The `tz.zone_processor` is rebound to the `zone_info` in case it was
 * previously bound to a different `zone_info`.
 *
 * Returns `odt` in an error state upon failure.
 */
void atc_time_zone_offset_date_time_from_epoch_seconds(
  const AtcTimeZone *tz,
  atc_time_t epoch_seconds,
  AtcOffsetDateTime *odt);

/**
 * Convert the PlainDateTime to AtcOffsetDateTime using the given time zone.
 * The `tz.zone_processor` is rebound to the `zone_info` in case it was
 * previously bound to a different `zone_info`.
 *
 * Returns `odt` in an error state upon failure.
 */
void atc_time_zone_offset_date_time_from_plain_date_time(
  const AtcTimeZone *tz,
  const AtcPlainDateTime *pdt,
  uint8_t disambiguate,
  AtcOffsetDateTime *odt);

/**
 * Populate the ZonedExtra using the given epoch seconds for the time zone.
 * The `tz.zone_processor` is rebound to the `zone_info` in case it was
 * previously bound to a different `zone_info`.
 *
 * Returns error code in `extra.type`.
 */
void atc_time_zone_zoned_extra_from_epoch_seconds(
  const AtcTimeZone *tz,
  atc_time_t epoch_seconds,
  AtcZonedExtra *extra);

/**
 * Populate the ZonedExtra using the given local date time for the time zone.
 * The `tz.zone_processor` is rebound to the `zone_info` in case it was
 * previously bound to a different `zone_info`.
 *
 * Returns error code in `extra.type`.
 */
void atc_time_zone_zoned_extra_from_plain_date_time(
  const AtcTimeZone *tz,
  const AtcPlainDateTime *pdt,
  uint8_t disambiguate,
  AtcZonedExtra *extra);

/** Print the name of the current time zone. */
void atc_time_zone_print(AtcStringBuffer *sb, const AtcTimeZone *tz);

#ifdef __cplusplus
}
#endif

#endif
