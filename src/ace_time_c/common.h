#ifndef ACE_TIME_C_COMMON_H
#define ACE_TIME_C_COMMON_H

#include <stddef.h> // size_t
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

/**
  * Copy at most dst_size characters from src to dst, while replacing all
  * occurrence of old_char with new_string. If new_string is "", then replace
  * with nothing. The resulting dst string is always NUL terminated.
  */
void atc_copy_replace_string(char *dst, size_t dst_size, const char *src,
    char old_char, const char *new_string);

#endif
