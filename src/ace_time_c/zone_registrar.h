/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_ZONE_REGISTRAR_H
#define ACE_TIME_C_ZONE_REGISTRAR_H

#include <stdint.h>
#include <stdbool.h>
#include "zone_info.h"
#include "common.h"

/** Determine if the registry is sorted by zone id. */
bool atc_registrar_is_registry_sorted(
    const struct AtcZoneInfo * const * registry,
    uint16_t size);

/**
 * Search the zone registry for the zone 'name'.
 * Return NULL if not found.
 */
const struct AtcZoneInfo *atc_registrar_find_by_name(
    const struct AtcZoneInfo * const * registry,
    uint16_t size,
    const char *name,
    bool is_sorted);

/**
 * Search the zone registry for the zone 'id'.
 * Return NULL if not found.
 */
const struct AtcZoneInfo *atc_registrar_find_by_id(
    const struct AtcZoneInfo * const * registry,
    uint16_t size,
    uint32_t zone_id,
    bool is_sorted);

#endif
