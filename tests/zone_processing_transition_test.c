/*
 * Unit tests for zone_processing.c. Much of this was adapted from
 * ExtendedZoneProcessorTransitionTest.ino from the AceTime library.
 */

#include <string.h>
#include <acetimec.h>
#include <acunit.h>

//---------------------------------------------------------------------------

static void check_sorted_transitions(AtcTransitionStorage *storage)
{
  AtcTransition **begin =
      atc_transition_storage_get_active_pool_begin(storage);
  AtcTransition **end =
      atc_transition_storage_get_active_pool_end(storage);

  AtcTransition *prev = NULL;
  for (AtcTransition **iter = begin; iter != end; iter++) {
    AtcTransition *t = *iter;
    if (prev) {
      ACU_ASSERT(prev->start_epoch_seconds <= t->start_epoch_seconds);
    }
    prev = t;
  }
}

static void check_unique_transitions(AtcTransitionStorage *storage)
{
  AtcTransition **begin =
      atc_transition_storage_get_active_pool_begin(storage);
  AtcTransition **end =
      atc_transition_storage_get_active_pool_end(storage);

  AtcTransition *prev = NULL;
  for (AtcTransition **iter = begin; iter != end; iter++) {
    AtcTransition *t = *iter;
    if (prev) {
      ACU_ASSERT(prev->start_epoch_seconds != t->start_epoch_seconds);
    }
    prev = t;
  }
}

static void validate_zone(
  AtcZoneProcessing *processing,
  const AtcZoneInfo *info,
  int16_t start_year,
  int16_t until_year)
{
  for (int16_t year = start_year; year < until_year; year++) {
    atc_processing_init_for_year(processing, info, year);
    AtcTransitionStorage *storage = &processing->transition_storage;
    ACU_ASSERT_NO_FATAL_FAILURE(check_sorted_transitions(storage));
    ACU_ASSERT_NO_FATAL_FAILURE(check_unique_transitions(storage));
  }
}

ACU_TEST(test_transitions_for_all_zones_all_years) {
  int16_t saved_epoch_year = atc_get_local_epoch_year();
  AtcZoneProcessing processing;
  atc_processing_init(&processing);

  for (uint16_t i = 0; i < kAtcZoneRegistrySize; i++) {
    const AtcZoneInfo *info = kAtcZoneRegistry[i];

    // Loop from start_year to until_year, in chunks of 100 years, shifting
    // the local_epoch_year by 100 years.
    for (
        int16_t start_year = kAtcZoneContext.start_year;
        start_year < kAtcZoneContext.until_year;
        start_year += 100) {

      atc_set_local_epoch_year(start_year + 50);
      int16_t until_year = start_year + 100;
      if (until_year > kAtcZoneContext.until_year) {
        until_year = kAtcZoneContext.until_year;
      }

      ACU_ASSERT_NO_FATAL_FAILURE(
          validate_zone(&processing, info, start_year, until_year));
    }
  }

  atc_set_local_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_transitions_for_all_zones_all_years);
  ACU_SUMMARY();
}
