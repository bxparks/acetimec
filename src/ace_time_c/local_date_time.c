#include "local_date_time.h"

#include <stdint.h>
#include "local_date.h"
#include "local_date_time.h"

// Offset to between 'year' and 'year_tiny'.
#define ATC_YEAR_TINY_OFFSET 2000

static int32_t to_epoch_seconds(uint8_t hour, uint8_t minute, uint8_t second)
{
  return ((hour * (int16_t) 60) + minute) * (int32_t) 60 + second;
}

int32_t atc_to_epoch_seconds(
  int16_t year, uint8_t month, uint8_t day,
  uint8_t hour, uint8_t minute, uint8_t second)
{
  if (year == INT16_MIN) return INT32_MIN;

  int8_t year_tiny = year - ATC_YEAR_TINY_OFFSET;
  int32_t days = atc_to_epoch_days(year_tiny, month, day);
  int32_t seconds = to_epoch_seconds(hour, minute, second);
  return days * 86400 + seconds;
}

void atc_from_epoch_seconds(
  int32_t epoch_seconds,
  int16_t *year, uint8_t *month, uint8_t *day,
  uint8_t *hour, uint8_t *minute, uint8_t *second)
{
  // TODO: Check for epoch_seconds which generates year<-127 or year>127.
  if (epoch_seconds == INT32_MIN) {
    *year = INT16_MIN;
    *month = 0;
    *day = 0;
    *hour = 0;
    *minute = 0;
    *second = 0;
    return;
  }

  // Integer floor-division towards -infinity
  int32_t days = (epoch_seconds < 0)
      ? (epoch_seconds + 1) / 86400 - 1
      : epoch_seconds / 86400;
  int32_t seconds = epoch_seconds - 86400 * days;

  // Extract (year, month day).
  int8_t year_tiny;
  atc_from_epoch_days(days, &year_tiny, month, day);
  *year = year_tiny + ATC_YEAR_TINY_OFFSET;

  // Extract (hour, minute, second). The compiler will combine the mod (%) and
  // division (/) operations into a single (dividend, remainder) function call.
  *second = seconds % 60;
  uint16_t minutes = seconds / 60;
  *minute = minutes % 60;
  *hour = minutes / 60;
}
