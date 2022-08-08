#include "local_date_time.h"

#include "common.h"
#include "local_date.h"
#include "local_date_time.h"

static atc_time_t hms_to_epoch_seconds(
    uint8_t hour, uint8_t minute, uint8_t second)
{
  return ((hour * (int16_t) 60) + minute) * (int32_t) 60 + second;
}

atc_time_t atc_local_date_time_to_epoch_seconds(
    const struct AtcLocalDateTime *ldt)
{
  if (ldt->year == kAtcInvalidYear) return kAtcInvalidEpochSeconds;

  int8_t year_tiny = ldt->year - kAtcEpochYear;
  int32_t days = atc_local_date_to_epoch_days(year_tiny, ldt->month, ldt->day);
  int32_t seconds = hms_to_epoch_seconds(ldt->hour, ldt->minute, ldt->second);
  return days * 86400 + seconds;
}

bool atc_local_date_time_from_epoch_seconds(
  atc_time_t epoch_seconds,
  struct AtcLocalDateTime *ldt)
{
  // TODO: Check for epoch_seconds which generates year<-127 or year>127.
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    ldt->year = kAtcInvalidYear;
    ldt->month = 0;
    ldt->day = 0;
    ldt->hour = 0;
    ldt->minute = 0;
    ldt->second = 0;
    return false;
  }

  // Integer floor-division towards -infinity
  int32_t days = (epoch_seconds < 0)
      ? (epoch_seconds + 1) / 86400 - 1
      : epoch_seconds / 86400;
  int32_t seconds = epoch_seconds - 86400 * days;

  // Extract (year, month day).
  int8_t year_tiny;
  atc_local_date_from_epoch_days(days, &year_tiny, &ldt->month, &ldt->day);
  ldt->year = year_tiny + kAtcEpochYear;

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  ldt->second = seconds % 60;
  uint16_t minutes = seconds / 60;
  ldt->minute = minutes % 60;
  ldt->hour = minutes / 60;

  return true;
}
