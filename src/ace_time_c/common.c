#include "common.h"

void atc_copy_replace_string(char *dst, size_t dst_size, const char *src,
    char old_char, const char *new_string)
{
  char c;
  while ((c = *src++) != '\0' && dst_size > 0) {
    if (c == old_char) {
      const char* s = new_string;
      while (*s != '\0' && dst_size > 0) {
        *dst++ = *s++;
        dst_size--;
      }
    } else {
      *dst++ = c;
      dst_size--;
    }
  }

  if (dst_size == 0) {
    --dst;
  }
  *dst = '\0';
}

uint32_t atc_djb2(const char *s) {
  uint32_t hash = 5381;
  uint8_t c;

  while ((c = *s++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}
