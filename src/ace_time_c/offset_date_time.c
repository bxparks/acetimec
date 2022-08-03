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
