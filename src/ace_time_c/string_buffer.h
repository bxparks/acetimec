#ifndef ACE_TIME_C_STRING_BUFFER_H
#define ACE_TIME_C_STRING_BUFFER_H

/**
 * @file string_buffer.h
 *
 * A simple set of integer to string conversion functions.
 */

#include <stdint.h>
#include <stddef.h>

typedef struct AtcStringBuffer {
  /** Pointer to base of the string buffer. */
  char *p;
  /** Total allocated size of buffer. */
  uint8_t capacity;
  /** Current buffer usage. */
  uint8_t size;
} AtcStringBuffer;

void atc_print_init(AtcStringBuffer *sb, char *p, uint8_t capacity);

void atc_print_end(AtcStringBuffer *sb);

void atc_print_char(AtcStringBuffer *sb, char c);

void atc_print_string(AtcStringBuffer *sb, const char *s);

void atc_print_uint16(AtcStringBuffer *sb, uint16_t n);

void atc_print_uint16_pad2(AtcStringBuffer *sb, uint16_t n);

void atc_print_uint16_pad4(AtcStringBuffer *sb, uint16_t n);

void atc_print_int16(AtcStringBuffer *sb, int16_t n);

#endif
