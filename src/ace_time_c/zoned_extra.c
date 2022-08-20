#include <string.h>
#include "zoned_extra.h"

bool atc_zoned_extra_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return false;
  bool status = atc_processing_transition_info_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      (struct AtcTransitionInfo *) extra);
  return status;
}
