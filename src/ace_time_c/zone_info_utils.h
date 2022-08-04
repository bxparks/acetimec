/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_INFO_UTILS_H
#define ACE_TIME_C_ZONE_INFO_UTILS_H

#include <stdint.h>
#include "zone_info.h"

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
