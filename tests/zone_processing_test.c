#include "acunit.h"
#include <acetimec.h>

acu_test(test_atc_zone_info_time_code_to_minutes)
{
  acu_assert(atc_zone_info_time_code_to_minutes(1, 1) == 16);
  acu_assert(atc_zone_info_time_code_to_minutes(2, 1) == 31);
  acu_assert(atc_zone_info_time_code_to_minutes(1, 2) == 17);

  acu_pass();
}

acu_test(test_atc_zone_info_modifier_to_suffix)
{
  acu_assert(atc_zone_info_modifier_to_suffix(kAtcSuffixW) == kAtcSuffixW);
  acu_assert(atc_zone_info_modifier_to_suffix(kAtcSuffixW | 0x1)
      == kAtcSuffixW);
  acu_assert(atc_zone_info_modifier_to_suffix(kAtcSuffixS | 0x8)
      == kAtcSuffixS);

  acu_pass();
}

acu_test(test_atc_compare_internal_date_time)
{
  struct AtcDateTime a = {0, 1, 1, kAtcSuffixW, 0};
  struct AtcDateTime b = {0, 1, 1, kAtcSuffixW, 0};
  acu_assert(atc_compare_internal_date_time(&a, &b) == 0);

  struct AtcDateTime bb = {0, 1, 1, kAtcSuffixS, 0};
  acu_assert(atc_compare_internal_date_time(&a, &bb) == 0);

  struct AtcDateTime c = {0, 1, 1, kAtcSuffixW, 1};
  acu_assert(atc_compare_internal_date_time(&a, &c) < 0);

  struct AtcDateTime d = {0, 1, 2, kAtcSuffixW, 0};
  acu_assert(atc_compare_internal_date_time(&a, &d) < 0);

  struct AtcDateTime e = {0, 2, 1, kAtcSuffixW, 0};
  acu_assert(atc_compare_internal_date_time(&a, &e) < 0);

  struct AtcDateTime f = {1, 1, 1, kAtcSuffixW, 0};
  acu_assert(atc_compare_internal_date_time(&a, &f) < 0);

  acu_pass();
}

//---------------------------------------------------------------------------

int acu_tests_run = 0;
int acu_tests_failed = 0;

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  acu_run_test(test_atc_zone_info_time_code_to_minutes);
  acu_run_test(test_atc_zone_info_modifier_to_suffix);
  acu_run_test(test_atc_compare_internal_date_time);
  acu_summary();
}
