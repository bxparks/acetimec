#include "acunit.h"
#include <acetimec.h>

//---------------------------------------------------------------------------

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
// Step 1
//---------------------------------------------------------------------------

ACU_TEST(test_atc_compare_era_to_year_month) {
  const struct AtcZoneEra era = {NULL, "", 0, 0, 0, 1, 2, 12, kAtcSuffixW};

  ACU_ASSERT(1 == atc_compare_era_to_year_month(&era, 0, 1));
  ACU_ASSERT(1 == atc_compare_era_to_year_month(&era, 0, 1));
  ACU_ASSERT(-1 == atc_compare_era_to_year_month(&era, 0, 2));
  ACU_ASSERT(-1 == atc_compare_era_to_year_month(&era, 0, 3));

  ACU_PASS();
}

ACU_TEST(test_atc_compare_era_to_year_month_equal) {
  const struct AtcZoneEra era2 = {NULL, "", 0, 0, 0, 1, 0, 0, kAtcSuffixW};

  ACU_ASSERT(0 == atc_compare_era_to_year_month(&era2, 0, 1));
  ACU_PASS();
}

//---------------------------------------------------------------------------
// A simplified version of America/Los_Angeles, using only simple ZoneEras
// (i.e. no references to a ZonePolicy). Valid only for 2018.
//---------------------------------------------------------------------------

static const struct AtcZoneContext kZoneContext = {
  2000 /*startYear*/,
  2020 /*untilYear*/,
  "testing" /*tzVersion*/,
  0 /*numFragments*/,
  NULL /*fragments*/,
};

// Create simplified ZoneEras which approximate America/Los_Angeles
static const struct AtcZoneEra kZoneEraAlmostLosAngeles[] = {
  {
    NULL,
    "PST" /*format*/,
    -32 /*offsetCode*/,
    0 + 4 /*deltaCode*/,
    19 /*untilYearTiny*/,
    3 /*untilMonth*/,
    10 /*untilDay*/,
    2*4 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/
  },
  {
    NULL,
    "PDT" /*format*/,
    -32 /*offsetCode*/,
    4 + 4 /*deltaCode*/,
    19 /*untilYearTiny*/,
    11 /*untilMonth*/,
    3 /*untilDay*/,
    2*4 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/
  },
  {
    NULL,
    "PST" /*format*/,
    -32 /*offsetCode*/,
    0 + 4 /*deltaCode*/,
    20 /*untilYearTiny*/,
    3 /*untilMonth*/,
    8 /*untilDay*/,
    2*4 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/
  },
};

static const struct AtcZoneInfo kZoneAlmostLosAngeles = {
  "Almost_Los_Angeles" /*name*/,
  0x70166020 /*zoneId*/,
  &kZoneContext /*zoneContext*/,
  3 /*numEras*/,
  kZoneEraAlmostLosAngeles /*eras*/,
};

//---------------------------------------------------------------------------
// A real ZoneInfo for America/Los_Angeles. Taken from zonedbx/zone_infos.cpp.
//---------------------------------------------------------------------------

static const struct AtcZoneRule kZoneRulesTestUS[] = {
  // Rule    US    1967    2006    -    Oct    lastSun    2:00    0    S
  {
    -33 /*fromYearTiny*/,
    6 /*toYearTiny*/,
    10 /*inMonth*/,
    7 /*onDayOfWeek*/,
    0 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    0 + 4 /*deltaCode*/,
    'S' /*letter*/,
  },
  // Rule    US    1976    1986    -    Apr    lastSun    2:00    1:00    D
  {
    -24 /*fromYearTiny*/,
    -14 /*toYearTiny*/,
    4 /*inMonth*/,
    7 /*onDayOfWeek*/,
    0 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    4 + 4 /*deltaCode*/,
    'D' /*letter*/,
  },
  // Rule    US    1987    2006    -    Apr    Sun>=1    2:00    1:00    D
  {
    -13 /*fromYearTiny*/,
    6 /*toYearTiny*/,
    4 /*inMonth*/,
    7 /*onDayOfWeek*/,
    1 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    4 + 4 /*deltaCode*/,
    'D' /*letter*/,
  },
  // Rule    US    2007    max    -    Mar    Sun>=8    2:00    1:00    D
  {
    7 /*fromYearTiny*/,
    126 /*toYearTiny*/,
    3 /*inMonth*/,
    7 /*onDayOfWeek*/,
    8 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    4 + 4 /*deltaCode*/,
    'D' /*letter*/,
  },
  // Rule    US    2007    max    -    Nov    Sun>=1    2:00    0    S
  {
    7 /*fromYearTiny*/,
    126 /*toYearTiny*/,
    11 /*inMonth*/,
    7 /*onDayOfWeek*/,
    1 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    0 + 4 /*deltaCode*/,
    'S' /*letter*/,
  },

};

static const struct AtcZonePolicy kPolicyTestUS = {
  kZoneRulesTestUS /*rules*/,
  NULL /* letters */,
  5 /*numRules*/,
  0 /* numLetters */,
};

static const struct AtcZoneEra kZoneEraTestLos_Angeles[] = {
  //             -8:00    US    P%sT
  {
    &kPolicyTestUS /*zonePolicy*/,
    "P%T" /*format*/,
    -32 /*offsetCode*/,
    0 + 4 /*deltaCode*/,
    127 /*untilYearTiny*/,
    1 /*untilMonth*/,
    1 /*untilDay*/,
    0 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/,
  },

};

static const struct AtcZoneInfo kZoneTestLosAngeles = {
  "America/Los_Angeles" /*name*/,
  0xb7f7e8f2 /*zoneId*/,
  &kZoneContext /*zoneContext*/,
  1 /*numEras*/,
  kZoneEraTestLos_Angeles /*eras*/,
};

//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_find_matches_simple) {
  struct AtcYearMonth start_ym = {18, 12};
  struct AtcYearMonth until_ym = {20, 2};
  const uint8_t kMaxMatches = 4;

  struct AtcMatchingEra matches[kMaxMatches];
  uint8_t num_matches = atc_processing_find_matches(
      &kZoneAlmostLosAngeles, start_ym, until_ym, matches, kMaxMatches);
  ACU_ASSERT(3 == num_matches);

  {
    struct AtcDateTuple *sdt = &matches[0].start_dt;
    ACU_ASSERT(sdt->year_tiny == 18);
    ACU_ASSERT(sdt->month == 12);
    ACU_ASSERT(sdt->day == 1);
    ACU_ASSERT(sdt->minutes == 0);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    struct AtcDateTuple *udt = &matches[0].until_dt;
    ACU_ASSERT(udt->year_tiny == 19);
    ACU_ASSERT(udt->month == 3);
    ACU_ASSERT(udt->day == 10);
    ACU_ASSERT(udt->minutes == 15*8);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const struct AtcZoneEra *eras =
        (const struct AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[0].era == &eras[0]);
  }

  {
    struct AtcDateTuple *sdt = &matches[1].start_dt;
    ACU_ASSERT(sdt->year_tiny == 19);
    ACU_ASSERT(sdt->month == 3);
    ACU_ASSERT(sdt->day == 10);
    ACU_ASSERT(sdt->minutes == 15*8);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    struct AtcDateTuple *udt = &matches[1].until_dt;
    ACU_ASSERT(udt->year_tiny == 19);
    ACU_ASSERT(udt->month == 11);
    ACU_ASSERT(udt->day == 3);
    ACU_ASSERT(udt->minutes == 15*8);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const struct AtcZoneEra *eras =
        (const struct AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[1].era == &eras[1]);
  }

  {
    struct AtcDateTuple *sdt = &matches[2].start_dt;
    ACU_ASSERT(sdt->year_tiny == 19);
    ACU_ASSERT(sdt->month == 11);
    ACU_ASSERT(sdt->day == 3);
    ACU_ASSERT(sdt->minutes == 15*8);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    struct AtcDateTuple *udt = &matches[2].until_dt;
    ACU_ASSERT(udt->year_tiny == 20);
    ACU_ASSERT(udt->month == 2);
    ACU_ASSERT(udt->day == 1);
    ACU_ASSERT(udt->minutes == 0);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const struct AtcZoneEra *eras =
        (const struct AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[2].era == &eras[2]);
  }

  ACU_PASS();
}

ACU_TEST(test_atc_processing_find_matches_named)
{
  struct AtcYearMonth start_ym = {18, 12};
  struct AtcYearMonth until_ym = {20, 2};
  const uint8_t kMaxMatches = 4;

  struct AtcMatchingEra matches[kMaxMatches];
  uint8_t num_matches = atc_processing_find_matches(
      &kZoneTestLosAngeles, start_ym, until_ym, matches, kMaxMatches);
  ACU_ASSERT(1 == num_matches);

  struct AtcDateTuple *sdt = &matches[0].start_dt;
  ACU_ASSERT(sdt->year_tiny == 18);
  ACU_ASSERT(sdt->month == 12);
  ACU_ASSERT(sdt->day == 1);
  ACU_ASSERT(sdt->minutes == 0);
  ACU_ASSERT(sdt->suffix == kAtcSuffixW);

  struct AtcDateTuple *udt = &matches[0].until_dt;
  ACU_ASSERT(udt->year_tiny == 20);
  ACU_ASSERT(udt->month == 2);
  ACU_ASSERT(udt->day == 1);
  ACU_ASSERT(udt->minutes == 0);
  ACU_ASSERT(udt->suffix == kAtcSuffixW);

  const struct AtcZoneEra *eras =
      (const struct AtcZoneEra *) kZoneTestLosAngeles.eras;
  ACU_ASSERT(matches[0].era == &eras[0]);

  ACU_PASS();
}

//---------------------------------------------------------------------------
// Step 2
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_processing_get_most_recent_prior_year);
  ACU_RUN_TEST(test_atc_processing_calc_interior_years);

  ACU_RUN_TEST(test_atc_compare_era_to_year_month);
  ACU_RUN_TEST(test_atc_compare_era_to_year_month_equal);
  ACU_RUN_TEST(test_atc_processing_find_matches_simple);
  ACU_RUN_TEST(test_atc_processing_find_matches_named);

  ACU_SUMMARY();
}
