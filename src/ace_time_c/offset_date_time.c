#include "local_date_time.h"
#include "offset_date_time.h"

atc_time_t atc_offset_date_time_to_epoch_seconds(
    const struct AtcOffsetDateTime *odt)
{
  const struct AtcLocalDateTime *ldt = (const struct AtcLocalDateTime *) odt;
  atc_time_t es = atc_local_date_time_to_epoch_seconds(ldt);
  if (es == kAtcInvalidEpochSeconds) return es;
  return es - odt->offset_minutes * 60;
}

bool atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    struct AtcOffsetDateTime *odt)
{
  if (epoch_seconds == kAtcInvalidEpochSeconds) return false;

  epoch_seconds += offset_minutes * 60;
  bool status = atc_local_date_time_from_epoch_seconds(
      epoch_seconds, (struct AtcLocalDateTime *) odt);
  if (! status) return status;

  odt->fold = 0;
  odt->offset_minutes = offset_minutes;
  return true;
}
