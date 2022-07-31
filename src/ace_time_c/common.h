#ifndef ACE_TIME_C_COMMON_H
#define ACE_TIME_C_COMMON_H

#include <stdint.h> // INT32_MIN

typedef int32_t atc_time_t;

enum {
  /** Base year of the AceTimeC epoch. */
  kAtcEpochYear = 2000,

  /** Sentinel value for invalid year. */
  kAtcInvalidYear = INT16_MIN,

  /** Sentinel value for invalid year. */
  kAtcInvalidYearTiny = -128,

  /** Invalid epoch seconds. */
  kAtcInvalidEpochSeconds = INT32_MIN,
};

#endif
