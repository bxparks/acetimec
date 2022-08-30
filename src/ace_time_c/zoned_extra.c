#include <string.h>
#include "zoned_extra.h"

int8_t atc_zoned_extra_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return kAtcErrGeneric;
  int8_t err = atc_processing_transition_info_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      (struct AtcTransitionInfo *) extra);
  return err;
}
