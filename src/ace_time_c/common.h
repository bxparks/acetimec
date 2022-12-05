/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file common.h
 *
 * Some common shared typedefs, constants, and functions.
 */

#ifndef ACE_TIME_C_COMMON_H
#define ACE_TIME_C_COMMON_H

#include <stddef.h> // size_t
#include <stdint.h> // INT32_MIN

#ifdef __cpluscplus
extern "C" {
#endif

/**
 * The number of seconds from the epoch of this library
 * which will usually be 2000-01-01 00:00:00 UTC.
 */
typedef int32_t atc_time_t;

enum {
  /** Error code to indicate success. */
  kAtcErrOk = 0,

  /** Error code to indicate generic failure. */
  kAtcErrGeneric = 1,
};

/**
 * Copy at most dst_size characters from src to dst, while replacing all
 * occurrence of old_char with new_string. If new_string is "", then replace
 * with nothing. The resulting dst string is always NUL terminated.
 */
void atc_copy_replace_string(char *dst, size_t dst_size, const char *src,
    char old_char, const char *new_string);

/**
 * Implement the djb2 hash algorithm as described in
 * https://stackoverflow.com/questions/7666509 and
 * http://www.cse.yorku.ca/~oz/hash.html.
 */
uint32_t atc_djb2(const char *s);

#ifdef __cpluscplus
}
#endif

#endif
