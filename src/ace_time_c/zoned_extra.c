/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <string.h>
#include "local_date.h"
#include "zone_processing.h"
#include "zoned_extra.h"

int8_t atc_zoned_extra_from_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;

  AtcFindResult result;
  int8_t err = atc_processing_find_by_epoch_seconds(
      processing, zone_info, epoch_seconds, &result);
  if (err) return err;

  extra->type = result.type;
  extra->std_offset_minutes = result.std_offset_minutes;
  extra->dst_offset_minutes = result.dst_offset_minutes;
  extra->req_std_offset_minutes = result.req_std_offset_minutes;
  extra->req_dst_offset_minutes = result.req_dst_offset_minutes;
  memcpy(extra->abbrev, result.abbrev, kAtcAbbrevSize);
  extra->abbrev[kAtcAbbrevSize - 1] = '\0';

  return kAtcErrOk;
}
