/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <string.h>
#include "zone_registrar.h"

bool atc_registrar_is_registry_sorted(
    const AtcZoneInfo * const * registry,
    uint16_t size)
{
  if (size == 0) return true;
  const AtcZoneInfo *prev = registry[0];
  for (uint16_t i = 1; i < size; i++) {
    const AtcZoneInfo *curr = registry[i];
    if (curr->zone_id < prev->zone_id) return false;
    prev = curr;
  }
  return true;
}

/**
 * Do a binary search for 'zone_id' in 'registry'. Return index if found,
 * or UINT16_MAX if not found.
 */
static uint16_t binary_search(
    const AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id)
{
  uint16_t a = 0;
  uint16_t b = size;
  while (1) {
    uint16_t diff = b - a;
    if (diff == 0) break;

    uint16_t c = a + diff / 2;
    const AtcZoneInfo *curr = registry[c];
    if (curr->zone_id == zone_id) return c;
    if (zone_id < curr->zone_id) {
      b = c;
    } else {
      a = c + 1;
    }
  }
  return UINT16_MAX;
}

static uint16_t linear_search(
    const AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id)
{
  for (uint16_t i = 0; i < size; i++) {
    const AtcZoneInfo *curr = registry[i];
    if (curr->zone_id == zone_id) return i;
  }
  return UINT16_MAX;
}

const AtcZoneInfo *atc_registrar_find_by_name(
    const AtcZoneInfo * const * registry,
    uint16_t size,
    const char *name,
    bool is_sorted)
{
  uint32_t zone_id = atc_djb2(name);
  const AtcZoneInfo *info = atc_registrar_find_by_id(
      registry, size, zone_id, is_sorted);
  if (! info) return info;

  // Verify that the name actually matches, in case of hash collision.
  if (strcmp(name, info->name) != 0) return NULL;

  return info;
}

const AtcZoneInfo *atc_registrar_find_by_id(
    const AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id,
    bool is_sorted)
{
  uint16_t index = (is_sorted)
      ? binary_search(registry, size, zone_id)
      : linear_search(registry, size, zone_id);
  if (index == UINT16_MAX) return NULL;
  const AtcZoneInfo *match = registry[index];
  return match;
}
