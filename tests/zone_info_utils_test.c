#include <string.h>
#include <acunit.h>
#include <acetimec.h>

// Check handling of LINK and ZONE entries.
ACU_TEST(test_zone_info_link)
{
  // Verify that US/Pacific is a link
  ACU_ASSERT(atc_zone_info_is_link(&kAtcZoneUS_Pacific));

  // And points to America/Los_Angeles.
  const AtcZoneInfo *actual_info = atc_zone_info_actual_info(
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

// Check handling of long and short names.
ACU_TEST(test_zone_info_names)
{
  // Check that they point to different names though.
  ACU_ASSERT(strcmp(
      atc_zone_info_zone_name(&kAtcZoneAmerica_Los_Angeles),
      "America/Los_Angeles") == 0);
  ACU_ASSERT(strcmp(
      atc_zone_info_zone_name(&kAtcZoneUS_Pacific),
      "US/Pacific") == 0);

  // Verify short names
  ACU_ASSERT(strcmp(
      atc_zone_info_short_name(&kAtcZoneAmerica_Los_Angeles),
      "Los_Angeles") == 0);
  ACU_ASSERT(strcmp(
      atc_zone_info_short_name(&kAtcZoneUS_Pacific),
      "Pacific") == 0);
}

ACU_TEST(test_zone_era)
{
  const AtcZoneEra *eras =
      (const AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const AtcZoneEra *era = &eras[0];

  ACU_ASSERT(-8*60 == atc_zone_era_std_offset_minutes(era));
  ACU_ASSERT(0*60 == atc_zone_era_dst_offset_minutes(era));
  ACU_ASSERT(0 == atc_zone_era_until_minutes(era));
  ACU_ASSERT(kAtcSuffixW == atc_zone_era_until_suffix(era));
}

ACU_TEST(test_zone_rule)
{
  const AtcZoneEra *eras =
      (const AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const AtcZoneEra *era = &eras[0];
  const AtcZonePolicy *policy = era->zone_policy;
  const AtcZoneRule *rule = &policy->rules[0];

  ACU_ASSERT(2*60 == atc_zone_rule_at_minutes(rule));
  ACU_ASSERT(kAtcSuffixW == atc_zone_rule_at_suffix(rule));
  ACU_ASSERT(0 == atc_zone_rule_dst_offset_minutes(rule));
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zone_info_link);
  ACU_RUN_TEST(test_zone_info_names);
  ACU_RUN_TEST(test_zone_era);
  ACU_RUN_TEST(test_zone_rule);
  ACU_SUMMARY();
}

