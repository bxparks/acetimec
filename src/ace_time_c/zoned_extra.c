/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <string.h>
#include "local_date.h"
#include "zoned_extra.h"

int8_t atc_zoned_extra_from_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;
  int8_t err = atc_processing_transition_info_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      (AtcTransitionInfo *) extra);
  return err;
}
