#include <string.h>
#include <acunit.h>
#include <acetimec.h>

// Check handling of LINK and ZONE entries.
ACU_TEST(test_zone_info_link)
{
  const AtcZoneInfo *zone_info = &kAtcZoneUS_Pacific;
  // Verify that US/Pacific is a link
  ACU_ASSERT(atc_zone_info_is_link(zone_info));

  // And points to America/Los_Angeles.
  const AtcZoneInfo *target_info = zone_info->target_info;
  ACU_ASSERT(target_info == &kAtcZoneAmerica_Los_Angeles);

  // And has the same number of eras.
  ACU_ASSERT(zone_info->num_eras == target_info->num_eras);

  // And points to the same era entries.
  uint8_t num_eras = zone_info->num_eras;
  for (uint8_t i = 0; i < num_eras; i++) {
    ACU_ASSERT(&zone_info->eras[i] == &target_info->eras[i]);
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

  ACU_ASSERT(-8*3600 == atc_zone_era_std_offset_seconds(era));
  ACU_ASSERT(0*3600 == atc_zone_era_dst_offset_seconds(era));
  ACU_ASSERT(0 == atc_zone_era_until_seconds(era));
  ACU_ASSERT(kAtcSuffixW == atc_zone_era_until_suffix(era));
}

ACU_TEST(test_zone_rule)
{
  const AtcZoneEra *eras =
      (const AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const AtcZoneEra *era = &eras[0];
  const AtcZonePolicy *policy = era->zone_policy;
  const AtcZoneRule *rule = &policy->rules[0];

  ACU_ASSERT(2*3600 == atc_zone_rule_at_seconds(rule));
  ACU_ASSERT(kAtcSuffixW == atc_zone_rule_at_suffix(rule));
  ACU_ASSERT(0 == atc_zone_rule_dst_offset_seconds(rule));
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

