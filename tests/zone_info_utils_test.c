#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_zone_era)
{
  const struct AtcZoneEra *eras =
      (const struct AtcZoneEra *) kAtcZoneAmerica_Los_Angeles.eras;
  const struct AtcZoneEra *era = &eras[0];

  ACU_ASSERT(-8*60 == atc_zone_era_std_offset_minutes(era));
  ACU_ASSERT(0*60 == atc_zone_era_dst_offset_minutes(era));
  ACU_ASSERT(0 == atc_zone_era_until_minutes(era));
  ACU_ASSERT(kAtcSuffixW == atc_zone_era_until_suffix(era));

  ACU_PASS();
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

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_zone_era);
  ACU_RUN_TEST(test_zone_rule);
  ACU_SUMMARY();
}

