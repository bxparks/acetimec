#ifndef ACE_TIME_C_COMMON_H
#define ACE_TIME_C_COMMON_H

typedef int32_t atc_time_t;

enum {
  /** Base year of the AceTimeC epoch. */
  kAtcEpochYear = 2000,

  /** Sentinel value for invalid year. */
  kAtcInvalidYearTiny = -128,
};

#endif
