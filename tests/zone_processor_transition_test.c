/*
 * Unit tests for zone_processor.c. Much of this was adapted from
 * ExtendedZoneProcessorTransitionTest.ino from the AceTime library.
 */

#include <string.h>
#include <acetimec.h>
#include <acunit.h>

//---------------------------------------------------------------------------

static void check_sorted_transitions(
    AcuContext *acu_context, AtcTransition **begin, AtcTransition **end)
{
  AtcTransition *prev = NULL;
  for (AtcTransition **iter = begin; iter != end; iter++) {
    AtcTransition *t = *iter;
    if (prev) {
      ACU_ASSERT(prev->start_epoch_seconds <= t->start_epoch_seconds);
    }
    prev = t;
  }
}

static void check_unique_transitions(
    AcuContext *acu_context, AtcTransition **begin, AtcTransition **end)
{
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
  AcuContext *acu_context,
  AtcZoneProcessor *processor,
  const AtcZoneInfo *info,
  int16_t start_year,
  int16_t until_year)
{
  atc_processor_init_for_zone_info(processor, info);
  for (int16_t year = start_year; year < until_year; year++) {
    int8_t err = atc_processor_init_for_year(processor, year);
    ACU_ASSERT(err == kAtcErrOk);

    AtcTransitionStorage *storage = &processor->transition_storage;
    AtcTransition **begin =
        atc_transition_storage_get_active_pool_begin(storage);
    AtcTransition **end =
        atc_transition_storage_get_active_pool_end(storage);

    // Verify that every year produces at least one transition
    ACU_ASSERT((end - begin) > 0);

    ACU_ASSERT_NO_FATAL_FAILURE(check_sorted_transitions(
        acu_context, begin, end));
    ACU_ASSERT_NO_FATAL_FAILURE(check_unique_transitions(
        acu_context, begin, end));
  }
}

ACU_TEST(test_transitions_for_all_zones_all_years) {
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  AtcZoneProcessor processor;
  atc_processor_init(&processor);

  for (uint16_t i = 0; i < kAtcZoneRegistrySize; i++) {
    const AtcZoneInfo *info = kAtcZoneRegistry[i];

    // Loop from start_year to until_year, in chunks of 100 years, shifting
    // the current_epoch_year by 100 years.
    for (
        int16_t start_year = kAtcZoneContext.start_year;
        start_year < kAtcZoneContext.until_year;
        start_year += 100) {

      atc_set_current_epoch_year(start_year + 50);
      int16_t until_year = start_year + 100;
      if (until_year > kAtcZoneContext.until_year) {
        until_year = kAtcZoneContext.until_year;
      }

      ACU_ASSERT_NO_FATAL_FAILURE(
          validate_zone(
              acu_context, &processor, info, start_year, until_year));
    }
  }

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_transitions_for_all_zones_all_years);
  ACU_SUMMARY();
}
