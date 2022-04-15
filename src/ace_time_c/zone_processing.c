/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdbool.h>
#include "zone_processing.h"

//---------------------------------------------------------------------------
// Internal helper methods.
//---------------------------------------------------------------------------

static void atc_processing_transition_storage_init(
  struct AtcTransitionStorage *transition_storage)
{
  for (int i = 0; i < kAtcTransitionStorageSize; i++) {
    transition_storage->transitions[i] =
        &transition_storage->transition_pool[i];
  }
  transition_storage->index_prior = 0;
  transition_storage->index_candidate = 0;
  transition_storage->index_free = 0;
}

int atc_zone_processing_find_matches(
  struct AtcZoneInfo *zone_info,
  struct AtcYearMonth start_ym,
  struct AtcYearMonth until_ym,
  struct AtcMatchingEra *matches,
  int num_matches)
{
  (void) zone_info;
  (void) start_ym;
  (void) until_ym;
  (void) matches;
  (void) num_matches;
  return 0;
}

//---------------------------------------------------------------------------

void atc_zone_processing_init(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info)
{
  processing->zone_info = NULL;
  processing->is_filled = 0;
  processing->num_matches = 0;
  processing->zone_info = zone_info;
}

static bool atc_zone_processing_is_filled_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  return processing->is_filled && (year == processing->year);
}

bool atc_zone_processing_init_for_year(
  struct AtcZoneProcessing *processing,
  int16_t year)
{
  if (atc_zone_processing_is_filled_for_year(processing, year)) return true;

  processing->year = year;
  processing->num_matches = 0;
  atc_processing_transition_storage_init(&processing->transition_storage);
  const struct AtcZoneContext *context = processing->zone_info->zone_context;
  if (year < context->start_year - 1 || context->until_year < year) {
    return false;
  }
  struct AtcYearMonth start_ym = { (int8_t) (year - kAtcEpochYear - 1), 12 };
  struct AtcYearMonth until_ym = { (int8_t) (year - kAtcEpochYear + 1), 2 };

  // Step 1: Find matches.
  int num_matches = atc_zone_processing_find_matches(
    processing->zone_info,
    start_ym,
    until_ym,
    processing->matches,
    kAtcMaxMatches);
  (void) num_matches;

  // Step 2: Create Transitions.
  // Step 3: Fix transition times.
  // Step 4: Generate start and until times.
  // Step 5: Calc abbreviations.

  return true;
}

void atc_zone_processing_init_for_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds)
{
  (void) processing;
  (void) zone_info;
  (void) epoch_seconds;
}

void atc_zone_processing_calc_offset_date_time(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  atc_time_t epoch_seconds,
  struct AtcOffsetDateTime *offset_dt)
{
  (void) processing;
  (void) zone_info;
  (void) epoch_seconds;
  (void) offset_dt;
}

atc_time_t atc_zone_processing_calc_epoch_seconds(
  struct AtcZoneProcessing *processing,
  struct AtcZoneInfo *zone_info,
  struct AtcLocalDateTime *local_dt)
{
  (void) processing;
  (void) zone_info;
  (void) local_dt;
  return 0;
}
