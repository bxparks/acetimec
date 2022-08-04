#include "acunit.h"
#include <acetimec.h>

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

ACU_TEST(test_atc_processing_calc_interior_years)
{
  const uint8_t kMaxInteriorYears = 4;
  int8_t interior_years[kMaxInteriorYears];

  uint8_t num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, -2, -1, 0, 2);
  ACU_ASSERT(0 == num);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 3, 5, 0, 2);
  ACU_ASSERT(0 == num);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, -2, 0, 0, 2);
  ACU_ASSERT(1 == num);
  ACU_ASSERT(0 == interior_years[0]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 2, 4, 0, 2);
  ACU_ASSERT(1 == num);
  ACU_ASSERT(2 == interior_years[0]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 1, 2, 0, 2);
  ACU_ASSERT(2 == num);
  ACU_ASSERT(1 == interior_years[0]);
  ACU_ASSERT(2 == interior_years[1]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, -1, 3, 0, 2);
  ACU_ASSERT(3 == num);
  ACU_ASSERT(0 == interior_years[0]);
  ACU_ASSERT(1 == interior_years[1]);
  ACU_ASSERT(2 == interior_years[2]);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_processing_get_most_recent_prior_year);
  ACU_RUN_TEST(test_atc_processing_calc_interior_years);
  ACU_SUMMARY();
}
