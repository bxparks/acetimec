#include "string_buffer.h"

void atc_buf_init(AtcStringBuffer *sb, char *p, uint8_t capacity)
{
  sb->p = p;
  sb->capacity = capacity;
  sb->size = 0;
}

void atc_buf_reset(AtcStringBuffer *sb)
{
  sb->size = 0;
}

void atc_buf_close(AtcStringBuffer *sb)
{
  if (sb->size >= sb->capacity) {
    sb->size = sb->capacity - 1;
  }
  sb->p[sb->size] = '\0';
}

void atc_print_char(AtcStringBuffer *sb, char c)
{
  if (sb->size >= sb->capacity) return;
  sb->p[sb->size] = c;
  sb->size++;
}

void atc_print_string(AtcStringBuffer *sb, const char *s)
{
  if (sb->size >= sb->capacity) return;
  for (uint8_t i = 0; i < sb->capacity - sb->size; i++) {
    if (s[i] == '\0') break;
    sb->p[sb->size] = s[i];
    sb->size++;
  }
}

void atc_print_uint16(AtcStringBuffer *sb, uint16_t n)
{
  // Calculate the digits in little-endian order.
  char tmp[5];
  uint8_t i = 0;
  do {
    uint16_t digit = n % 10;
    n /= 10;
    tmp[i] = digit + '0';
    i++;
  } while (n != 0);

  // Print the digits in reversed, big-endian order for humans.
  while (i--) {
    atc_print_char(sb, tmp[i]);
  }
}

void atc_print_uint16_pad2(AtcStringBuffer *sb, uint16_t n)
{
  if (n < 10) {
    atc_print_char(sb, '0');
  }
  atc_print_uint16(sb, n);
}

void atc_print_uint16_pad4(AtcStringBuffer *sb, uint16_t n)
{
  if (n < 10) {
    atc_print_char(sb, '0');
  }
  if (n < 100) {
    atc_print_char(sb, '0');
  }
  if (n < 1000) {
    atc_print_char(sb, '0');
  }
  atc_print_uint16(sb, n);
}

void atc_print_int16(AtcStringBuffer *sb, int16_t n)
{
  if (n == INT16_MIN) {
    atc_print_string(sb, "-32768");
  } else {
    atc_print_char(sb, '-');
    atc_print_uint16(sb, -n);
  }
}
