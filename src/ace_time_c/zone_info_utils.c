#include <stdbool.h>
#include <string.h>
#include "zone_info_utils.h"

//---------------------------------------------------------------------------

bool atc_zone_info_is_link(const struct AtcZoneInfo *info)
{
  return info->num_eras == 0;
}

const struct AtcZoneInfo *atc_zone_info_actual_info(
    const struct AtcZoneInfo *info)
{
  if (atc_zone_info_is_link(info)) {
    return (const struct AtcZoneInfo *) (info->eras);
  } else {
    return info;
  }
}

uint8_t atc_zone_info_num_eras(const struct AtcZoneInfo *info)
{
  const struct AtcZoneInfo *actual_info = atc_zone_info_actual_info(info);
  return actual_info->num_eras;
}

const struct AtcZoneEra *atc_zone_info_era(
    const struct AtcZoneInfo *info, uint8_t i)
{
  const struct AtcZoneInfo *actual_info = atc_zone_info_actual_info(info);
  const struct AtcZoneEra *eras = (const struct AtcZoneEra *) actual_info->eras;
  return &eras[i];
}

const char *atc_zone_info_zone_name(
    const struct AtcZoneInfo *info)
{
  return info->name;
}

const char *atc_zone_info_short_name(
    const struct AtcZoneInfo *info)
{
  size_t len = strlen(info->name);
  const char* begin = info->name + len;
  while (len--) {
    begin--;
    char c = *begin;
    if (c == '/') {
      begin++;
      break;
    }
  }
  return begin;
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

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
