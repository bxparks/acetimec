#ifndef ACE_TIME_C_STRING_BUFFER_H
#define ACE_TIME_C_STRING_BUFFER_H

/**
 * @file string_buffer.h
 *
 * A simple set of integer to string conversion functions.
 */

#include <stdint.h>
#include <stddef.h>

struct AtcStringBuffer {
  /** Pointer to base of the string buffer. */
  char *p;
  /** Total allocated size of buffer. */
  uint8_t capacity;
  /** Current buffer usage. */
  uint8_t size;
};

void atc_print_init(struct AtcStringBuffer *sb, char *p, uint8_t capacity);

void atc_print_end(struct AtcStringBuffer *sb);

void atc_print_char(struct AtcStringBuffer *sb, char c);

void atc_print_string(struct AtcStringBuffer *sb, const char *s);

void atc_print_uint16(struct AtcStringBuffer *sb, uint16_t n);

void atc_print_uint16_pad2(struct AtcStringBuffer *sb, uint16_t n);

void atc_print_uint16_pad4(struct AtcStringBuffer *sb, uint16_t n);

void atc_print_int16(struct AtcStringBuffer *sb, int16_t n);

#endif
