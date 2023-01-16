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

typedef struct AtcTimeZone {
  const AtcZoneInfo *zone_info;
  AtcZoneProcessor *zone_processor;
} AtcTimeZone;

#ifdef __cplusplus
}
#endif

#endif
