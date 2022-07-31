#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_atc_zone_info_time_code_to_minutes)
{
  ACU_ASSERT(atc_zone_info_time_code_to_minutes(1, 1) == 16);
  ACU_ASSERT(atc_zone_info_time_code_to_minutes(2, 1) == 31);
  ACU_ASSERT(atc_zone_info_time_code_to_minutes(1, 2) == 17);

  ACU_PASS();
}

ACU_TEST(test_atc_zone_info_modifier_to_suffix)
{
  ACU_ASSERT(atc_zone_info_modifier_to_suffix(kAtcSuffixW) == kAtcSuffixW);
  ACU_ASSERT(atc_zone_info_modifier_to_suffix(kAtcSuffixW | 0x1)
      == kAtcSuffixW);
  ACU_ASSERT(atc_zone_info_modifier_to_suffix(kAtcSuffixS | 0x8)
      == kAtcSuffixS);

  ACU_PASS();
}

ACU_TEST(test_atc_processing_compare_date_tuple)
{
  struct AtcDateTuple a = {0, 1, 1, kAtcSuffixW, 0};
  struct AtcDateTuple b = {0, 1, 1, kAtcSuffixW, 0};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &b) == 0);

  struct AtcDateTuple bb = {0, 1, 1, kAtcSuffixS, 0};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &bb) == 0);

  struct AtcDateTuple c = {0, 1, 1, kAtcSuffixW, 1};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &c) < 0);

  struct AtcDateTuple d = {0, 1, 2, kAtcSuffixW, 0};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &d) < 0);

  struct AtcDateTuple e = {0, 2, 1, kAtcSuffixW, 0};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &e) < 0);

  struct AtcDateTuple f = {1, 1, 1, kAtcSuffixW, 0};
  ACU_ASSERT(atc_processing_compare_date_tuple(&a, &f) < 0);

  ACU_PASS();
}

ACU_TEST(test_atc_processing_get_most_recent_prior_year)
{
  int8_t year_tiny;

  year_tiny = atc_processing_get_most_recent_prior_year(-2, -1, 0, 2);
  ACU_ASSERT(-1 == year_tiny);

  year_tiny = atc_processing_get_most_recent_prior_year(3, 5, 0, 2);
  ACU_ASSERT(kAtcInvalidYearTiny == year_tiny);

  year_tiny = atc_processing_get_most_recent_prior_year(-2, 0, 0, 2);
  ACU_ASSERT(-1 == year_tiny);

  year_tiny = atc_processing_get_most_recent_prior_year(2, 4, 0, 2);
  ACU_ASSERT(kAtcInvalidYearTiny == year_tiny);

  year_tiny = atc_processing_get_most_recent_prior_year(1, 2, 0, 2);
  ACU_ASSERT(kAtcInvalidYearTiny == year_tiny);

  year_tiny = atc_processing_get_most_recent_prior_year(-1, 3, 0, 2);
  ACU_ASSERT(-1 == year_tiny);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  ACU_RUN_TEST(test_atc_zone_info_time_code_to_minutes);
  ACU_RUN_TEST(test_atc_zone_info_modifier_to_suffix);
  ACU_RUN_TEST(test_atc_processing_compare_date_tuple);
  ACU_RUN_TEST(test_atc_processing_get_most_recent_prior_year);
  ACU_SUMMARY();
}
