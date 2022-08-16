#include <string.h>
#include "zoned_extra.h"

bool atc_zoned_extra_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedExtra *extra)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return false;
  struct AtcTransitionInfo ti;
  atc_processing_transition_info_from_epoch_seconds(
      processing,
      zone_info,
      epoch_seconds,
      &ti);
  extra->utc_offset_minutes = ti.std_offset_minutes + ti.dst_offset_minutes;
  extra->dst_offset_minutes = ti.dst_offset_minutes;
  strncpy(extra->abbrev, ti.abbrev, kAtcAbbrevSize);
  extra->abbrev[kAtcAbbrevSize - 1] = '\0';
  return true;
}
