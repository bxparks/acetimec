/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file zone_registrar.h
 *
 * Functions that search for specific time zones by name or by ID over a given
 * set of zones defined by a zone registry.
 */

#ifndef ACE_TIME_C_ZONE_REGISTRAR_H
#define ACE_TIME_C_ZONE_REGISTRAR_H

#include <stdint.h>
#include <stdbool.h>
#include "../zoneinfo/zone_info.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Data structure used by the registrar to manage a given zone registry. */
typedef struct AtcZoneRegistrar {
  /** An array of pointers to AtcZoneInfo data structures. */
  const AtcZoneInfo * const * registry;

  /** Size of the registry. */
  uint16_t size;

  /** True if the registry is sorted according by zone_id. */
  bool is_sorted;
} AtcZoneRegistrar;

/**Initialize the given registrar data structure with the given registry. */
void atc_registrar_init(
    AtcZoneRegistrar *registrar,
    const AtcZoneInfo * const * registry,
    uint16_t size);

/** Determine if the registry is sorted by zone id. */
bool atc_registrar_is_registry_sorted(
    const AtcZoneInfo * const * registry,
    uint16_t size);

/**
 * Search the zone registry for the zone 'name'.
 * Return NULL if not found.
 */
const AtcZoneInfo *atc_registrar_find_by_name(
    const AtcZoneRegistrar *registrar,
    const char *name);

/**
 * Search the zone registry for the zone 'id'.
 * Return NULL if not found.
 */
const AtcZoneInfo *atc_registrar_find_by_id(
    const AtcZoneRegistrar *registrar,
    uint32_t zone_id);

#ifdef __cplusplus
}
#endif

#endif
