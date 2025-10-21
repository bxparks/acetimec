/*
 * MIT License
 * Copyright (c) 2025 Brian T. Park
 */

/**
 * @file zone_info.h
 *
 * The data structures in the zone database, with the AtcZoneInfo representing a
 * specific time zone. There are 2 slightly different persistence formats
 * defined in this library:
 *
 * 1) zone_info_high.h: 1 second resolution for everything, equivalent to
 * ZoneInfoHigh.h in the AceTime library. Selected by setting
 * ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_HIGH.
 *
 * 2) zone_info_mid.h: 1 minute resolution for AT, UNTIL STDOFF; 15 minute
 * resolution for SAVE, equivalent to ZoneInfoMid.h in AceTime library. Selected
 * by settings ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_MID. This format
 * has been only sparsely tested.
 *
 * The AceTimeLib library also defines a low-resolution format through
 * ZoneInfoLow.h (ACE_TIME_C_ZONEDB_RES=ACE_TIME_C_ZONEDB_RES_LOW). It is
 * unlikely that acetimec will offer this format, because the C-language does
 * not offer sufficient abtraction and encapsulation features to support these
 * different storage formats in a single library.
 */

#ifndef ACE_TIME_C_ZONE_INFO_H
#define ACE_TIME_C_ZONE_INFO_H

/**
 * There are 3 possible storage formats for the zonedb database. The acetimec
 * library implements 2: MID and HIGH. (The AceTime library implements LOW and
 * HIGH).
 */
#define ACE_TIME_C_ZONEDB_RES_HIGH 1 // default
#define ACE_TIME_C_ZONEDB_RES_MID 2 // implemented, but not well tested
#define ACE_TIME_C_ZONEDB_RES_LOW 3 // not implemented

#ifndef ACE_TIME_C_ZONEDB_RES
/**
 * If not defined externally (e.g. through a Makefile), set
 * ACE_TIME_C_ZONEDB_RES to use high-resolution data structures defined by
 * `zone_info_high.h`.
 */
#define ACE_TIME_C_ZONEDB_RES ACE_TIME_C_ZONEDB_RES_HIGH
#endif

#if ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_HIGH
  #include "zone_info_high.h"
#elif ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_MID
  #include "zone_info_mid.h"
#else
  #error "Unknown ACE_TIME_C_ZONEDB_RES"
#endif

#endif
