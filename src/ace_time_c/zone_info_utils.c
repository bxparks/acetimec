#include "zone_info_utils.h"

int16_t atc_zone_era_std_offset_minutes(
    const struct AtcZoneEra *era)
{
  return (era->offset_code * 15) + (((uint8_t) era->delta_code & 0xf0) >> 4);
}

int16_t atc_zone_era_dst_offset_minutes(
    const struct AtcZoneEra *era)
{
  return ((int8_t)((uint8_t)era->delta_code & 0x0f) - 4) * 15;
}

int16_t atc_zone_era_until_minutes(
    const struct AtcZoneEra *era)
{
  return era->until_time_code * (uint16_t) 15
      + (era->until_time_modifier & 0x0f);
}

uint8_t atc_zone_era_until_suffix(
    const struct AtcZoneEra *era)
{
  return era->until_time_modifier & 0xf0;
}

int16_t atc_zone_rule_at_minutes(
    const struct AtcZoneRule *rule)
{
  return rule->at_time_code * (uint16_t) 15
      + (rule->at_time_modifier & 0x0f);
}

uint8_t atc_zone_rule_at_suffix(
    const struct AtcZoneRule *rule)
{
  return rule->at_time_modifier & 0xf0;
}

int16_t atc_zone_rule_dst_offset_minutes(
    const struct AtcZoneRule *rule)
{
  return ((int8_t)((uint8_t)rule->delta_code & 0x0f) - 4) * 15;
}
