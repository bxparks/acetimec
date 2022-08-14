/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <string.h>
#include "zone_registrar.h"

const struct AtcZoneInfo *atc_registrar_find_by_name(
    const struct AtcZoneInfo * const * registry,
    uint16_t size,
    const char *name)
{
  uint32_t zone_id = atc_djb2(name);
  const struct AtcZoneInfo *info = atc_registrar_find_by_id(
      registry, size, zone_id);
  if (! info) return info;

  // Verify that the name actually matches, in case of hash collision.
  if (strcmp(name, info->name) != 0) return NULL;

  return info;
}

/**
 * Do a binary search for 'zone_id' in 'registry'. Return index if found,
 * or UINT16_MAX if not found.
 */
static uint16_t binary_search(
    const struct AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id)
{
  uint16_t a = 0;
  uint16_t b = size;
  while (1) {
    uint16_t diff = b - a;
    if (diff == 0) break;

    uint16_t c = a + diff / 2;
    const struct AtcZoneInfo *current = registry[c];
    if (current->zone_id == zone_id) return c;
    if (zone_id < current->zone_id) {
      b = c;
    } else {
      a = c + 1;
    }
  }
  return UINT16_MAX;
}

const struct AtcZoneInfo *atc_registrar_find_by_id(
    const struct AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id)
{
  uint16_t index = binary_search(registry, size, zone_id);
  if (index == UINT16_MAX) return NULL;
  const struct AtcZoneInfo *match = registry[index];
  return match;
}
