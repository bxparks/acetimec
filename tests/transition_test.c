#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_atc_date_tuple_compare)
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

ACU_TEST(test_atc_date_tuple_subtract)
{
  {
    struct AtcDateTuple dta = {0, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00
    struct AtcDateTuple dtb = {0, 1, 1, 1, kAtcSuffixW}; // 2000-01-01 00:01
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT(-60 == diff);
  }

  {
    struct AtcDateTuple dta = {0, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00
    struct AtcDateTuple dtb = {0, 1, 2, 0, kAtcSuffixW}; // 2000-01-02 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 == diff);
  }

  {
    struct AtcDateTuple dta = {0, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00
    struct AtcDateTuple dtb = {0, 2, 1, 0, kAtcSuffixW}; // 2000-02-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 31 == diff); // January has 31 days
  }

  {
    struct AtcDateTuple dta = {0, 2, 1, 0, kAtcSuffixW}; // 2000-02-01 00:00
    struct AtcDateTuple dtb = {0, 3, 1, 0, kAtcSuffixW}; // 2000-03-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 29 == diff); // Feb 2000 is leap, 29 days
  }

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_date_tuple_compare);
  ACU_RUN_TEST(test_atc_date_tuple_subtract);
  ACU_SUMMARY();
}
