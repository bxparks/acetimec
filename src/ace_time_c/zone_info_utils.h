/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_INFO_UTILS_H
#define ACE_TIME_C_ZONE_INFO_UTILS_H

#include <stdint.h>
#include "zone_info.h"

/** Return true if the info is actually a Link instead of a Zone. */
bool atc_zone_info_is_link(
    const struct AtcZoneInfo *info);

/** Return the info (Zone) or the target info (Link). */
const struct AtcZoneInfo *atc_zone_info_actual_info(
    const struct AtcZoneInfo *info);

/** Return the effective number of eras, after resolving the Link. */
uint8_t atc_zone_info_num_eras(
    const struct AtcZoneInfo *info);

/** Return the ZoneEra after resolving the Link. */
const struct AtcZoneEra *atc_zone_info_era(
    const struct AtcZoneInfo *info, uint8_t i);

int16_t atc_zone_era_std_offset_minutes(
    const struct AtcZoneEra *era);

int16_t atc_zone_era_dst_offset_minutes(
    const struct AtcZoneEra *era);

int16_t atc_zone_era_until_minutes(
    const struct AtcZoneEra *era);

uint8_t atc_zone_era_until_suffix(
    const struct AtcZoneEra *era);

int16_t atc_zone_rule_at_minutes(
    const struct AtcZoneRule *rule);

uint8_t atc_zone_rule_at_suffix(
    const struct AtcZoneRule *rule);

int16_t atc_zone_rule_dst_offset_minutes(
    const struct AtcZoneRule *rule);

#endif
