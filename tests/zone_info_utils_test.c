#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_zone_info)
{
  // Verify that US/Pacific is a link
  ACU_ASSERT(atc_zone_info_is_link(&kAtcZoneUS_Pacific));

  // And points to America/Los_Angeles.
  const struct AtcZoneInfo *actual_info = atc_zone_info_actual_info(
      &kAtcZoneUS_Pacific);
  ACU_ASSERT(actual_info == &kAtcZoneAmerica_Los_Angeles);

  // And has the same number of eras.
  ACU_ASSERT(
      atc_zone_info_num_eras(&kAtcZoneAmerica_Los_Angeles)
      == atc_zone_info_num_eras(&kAtcZoneUS_Pacific));

  // And points to the same era entries.
  uint8_t num_eras = atc_zone_info_num_eras(&kAtcZoneAmerica_Los_Angeles);
  for (uint8_t i = 0; i < num_eras; i++) {
    ACU_ASSERT(
        atc_zone_info_era(&kAtcZoneAmerica_Los_Angeles, i)
        == atc_zone_info_era(&kAtcZoneUS_Pacific, i));
  }
}

ACU_TEST(test_zone_era)
{
  const struct AtcZoneEra *eras =
      (const struct AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const struct AtcZoneEra *era = &eras[0];

  ACU_ASSERT(-8*60 == atc_zone_era_std_offset_minutes(era));
  ACU_ASSERT(0*60 == atc_zone_era_dst_offset_minutes(era));
  ACU_ASSERT(0 == atc_zone_era_until_minutes(era));
  ACU_ASSERT(kAtcSuffixW == atc_zone_era_until_suffix(era));
}

ACU_TEST(test_zone_rule)
{
  const struct AtcZoneEra *eras =
      (const struct AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const struct AtcZoneEra *era = &eras[0];
  const struct AtcZonePolicy *policy = era->zone_policy;
  const struct AtcZoneRule *rule = &policy->rules[0];

  ACU_ASSERT(2*60 == atc_zone_rule_at_minutes(rule));
  ACU_ASSERT(kAtcSuffixW == atc_zone_rule_at_suffix(rule));
  ACU_ASSERT(0 == atc_zone_rule_dst_offset_minutes(rule));
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_zone_info);
  ACU_RUN_TEST(test_zone_era);
  ACU_RUN_TEST(test_zone_rule);
  ACU_SUMMARY();
}

