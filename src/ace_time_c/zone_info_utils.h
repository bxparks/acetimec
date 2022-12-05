/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zone_info_utils.h
 *
 * Most fields in the zonedb database (defined by the data types in
 * zone_info.h) are meant to be accessed directly through a raw pointer. Some of
 * the fields are encoded in non-trival ways (usually to save memory). For these
 * fields, the following accessor functions know how to parse and interpret
 * those fields into a form that can be consumed more readily.
 */

#ifndef ACE_TIME_C_ZONE_INFO_UTILS_H
#define ACE_TIME_C_ZONE_INFO_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include "zone_info.h"

#ifdef __cpluscplus
extern "C" {
#endif

/**
 * A Zone entry and a Link entry are encoded by the same AceZoneInfo data
 * type. A Link entry is represented by the same header, but the `eras`
 * pointer points to the target Zone entry of the Link. This function returns
 * true if the info is actually a Link instead of a Zone.
 */
bool atc_zone_info_is_link(const AtcZoneInfo *info);

/** Return the info (info is a Zone) or the target info (if info is a Link). */
const AtcZoneInfo *atc_zone_info_actual_info(const AtcZoneInfo *info);

/** Return the effective number of eras, after resolving the Link. */
uint8_t atc_zone_info_num_eras(const AtcZoneInfo *info);

/** Return the full zone name of the given zone info. */
const char *atc_zone_info_zone_name(const AtcZoneInfo *info);

/**
 * Return the short zone name of the given zone info. The short name is the
 * final component of the full zone name after the last '/'. For example, the
 * short name of "America/Los_Angeles" is "Los_Angeles".
 */
const char *atc_zone_info_short_name(const AtcZoneInfo *info);

/** Return the ZoneEra after resolving the Link. */
const AtcZoneEra *atc_zone_info_era(const AtcZoneInfo *info, uint8_t i);

/** Return the standard offset of the given era in minutes. */
int16_t atc_zone_era_std_offset_minutes(const AtcZoneEra *era);

/** Return the DST offset of the given era in minutes. */
int16_t atc_zone_era_dst_offset_minutes(const AtcZoneEra *era);

/** Return the UNTIL time in minutes. */
int16_t atc_zone_era_until_minutes(const AtcZoneEra *era);

/** Return the UNTIL time suffix ('w', 's', 'u') */
uint8_t atc_zone_era_until_suffix(const AtcZoneEra *era);

/** Return the AT time in minutes. */
int16_t atc_zone_rule_at_minutes(const AtcZoneRule *rule);

/** Return the AT time suffix ('w', 's', 'u') */
uint8_t atc_zone_rule_at_suffix(const AtcZoneRule *rule);

/** Return the DST offset in minutes. */
int16_t atc_zone_rule_dst_offset_minutes(const AtcZoneRule *rule);

#ifdef __cpluscplus
}
#endif

#endif
