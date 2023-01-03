/*
 * Unit tests for zone_processing.c. Much of this was adapted from
 * ExtendedZoneProcessorTest.ino from the AceTime library.
 */

#include <string.h>
#include <acetimec.h>
#include <acunit.h>

//---------------------------------------------------------------------------
// Step 1
//---------------------------------------------------------------------------

ACU_TEST(test_atc_compare_era_to_year_month) {
  const AtcZoneEra era = {NULL, "", 0, 0, 2000/*y*/, 1, 2, 12, kAtcSuffixW};

  ACU_ASSERT(1 == atc_compare_era_to_year_month(&era, 2000, 1));
  ACU_ASSERT(1 == atc_compare_era_to_year_month(&era, 2000, 1));
  ACU_ASSERT(-1 == atc_compare_era_to_year_month(&era, 2000, 2));
  ACU_ASSERT(-1 == atc_compare_era_to_year_month(&era, 2000, 3));
}

ACU_TEST(test_atc_compare_era_to_year_month_equal) {
  const AtcZoneEra era2 = {NULL, "", 0, 0, 2000/*y*/, 1, 0, 0, kAtcSuffixW};
  ACU_ASSERT(0 == atc_compare_era_to_year_month(&era2, 2000, 1));
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_create_matching_era) {
  // 14-month interval, from 2000-12 until 2002-02
  AtcYearMonth start_ym = {2000, 12};
  AtcYearMonth until_ym = {2002, 2};

  // UNTIL = 2000-12-02 3:00
  const AtcZoneEra era1 =
      {NULL, "", 0, 0, 2000 /*y*/, 12/*m*/, 2/*d*/, 3*(60/15),
      kAtcSuffixW};

  // UNTIL = 2001-02-03 4:00
  const AtcZoneEra era2 =
      {NULL, "", 0, 0, 2001 /*y*/, 2/*m*/, 3/*d*/, 4*(60/15),
      kAtcSuffixW};

  // UNTIL = 2002-10-11 4:00
  const AtcZoneEra era3 =
      {NULL, "", 0, 0, 2002 /*y*/, 10/*m*/, 11/*d*/, 4*(60/15),
      kAtcSuffixW};

  // No previous matching era, so start_dt is set to start_ym.
  AtcMatchingEra match1;
  atc_create_matching_era(
      &match1,
      NULL /*prevMatch*/,
      &era1 /*era*/,
      start_ym,
      until_ym);
  ACU_ASSERT(match1.start_dt.year == 2000);
  ACU_ASSERT(match1.start_dt.month == 12);
  ACU_ASSERT(match1.start_dt.day == 1);
  ACU_ASSERT(match1.start_dt.minutes == 60*0);
  ACU_ASSERT(match1.start_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match1.until_dt.year == 2000);
  ACU_ASSERT(match1.until_dt.month == 12);
  ACU_ASSERT(match1.until_dt.day == 2);
  ACU_ASSERT(match1.until_dt.minutes == 60*3);
  ACU_ASSERT(match1.until_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match1.era == &era1);

  // start_dt is set to the prevMatch.until_dt.
  // until_dt is < until_ym, so is retained.
  AtcMatchingEra match2;
  atc_create_matching_era(
      &match2,
      &match1,
      &era2 /*era*/,
      start_ym,
      until_ym);
  ACU_ASSERT(match2.start_dt.year == 2000);
  ACU_ASSERT(match2.start_dt.month == 12);
  ACU_ASSERT(match2.start_dt.day == 2);
  ACU_ASSERT(match2.start_dt.minutes == 60*3);
  ACU_ASSERT(match2.start_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match2.until_dt.year == 2001);
  ACU_ASSERT(match2.until_dt.month == 2);
  ACU_ASSERT(match2.until_dt.day == 3);
  ACU_ASSERT(match2.until_dt.minutes == 60*4);
  ACU_ASSERT(match2.until_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match2.era == &era2);

  // start_dt is set to the prevMatch.until_dt.
  // until_dt is > until_ym so truncated to until_ym.
  AtcMatchingEra match3;
  atc_create_matching_era(
      &match3,
      &match2,
      &era3 /*era*/,
      start_ym,
      until_ym);
  ACU_ASSERT(match3.start_dt.year == 2001);
  ACU_ASSERT(match3.start_dt.month == 2);
  ACU_ASSERT(match3.start_dt.day == 3);
  ACU_ASSERT(match3.start_dt.minutes == 60*4);
  ACU_ASSERT(match3.start_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match3.until_dt.year == 2002);
  ACU_ASSERT(match3.until_dt.month == 2);
  ACU_ASSERT(match3.until_dt.day == 1);
  ACU_ASSERT(match3.until_dt.minutes == 60*0);
  ACU_ASSERT(match3.until_dt.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(match3.era == &era3);
}

//---------------------------------------------------------------------------
// A simplified version of America/Los_Angeles, using only simple ZoneEras
// (i.e. no references to a ZonePolicy). Valid only for 2018.
//---------------------------------------------------------------------------

static const AtcZoneContext kZoneContext = {
  2000 /*startYear*/,
  2020 /*untilYear*/,
  "testing" /*tzVersion*/,
  0 /*numFragments*/,
  NULL /*fragments*/,
};

// Create simplified ZoneEras which approximate America/Los_Angeles
static const AtcZoneEra kZoneEraAlmostLosAngeles[] = {
  {
    NULL,
    "PST" /*format*/,
    -32 /*offsetCode*/,
    0 + 4 /*deltaCode*/,
    2019 /*untilYear*/,
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
    2019 /*untilYear*/,
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
    2020 /*untilYear*/,
    3 /*untilMonth*/,
    8 /*untilDay*/,
    2*4 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/
  },
};

static const AtcZoneInfo kZoneAlmostLosAngeles = {
  "Almost_Los_Angeles" /*name*/,
  0x70166020 /*zoneId*/,
  &kZoneContext /*zoneContext*/,
  3 /*numEras*/,
  kZoneEraAlmostLosAngeles /*eras*/,
};

//---------------------------------------------------------------------------
// A real ZoneInfo for America/Los_Angeles. Taken from zonedbx/zone_infos.cpp.
//---------------------------------------------------------------------------

static const AtcZoneRule kZoneRulesTestUS[] = {
  // Rule    US    1967    2006    -    Oct    lastSun    2:00    0    S
  {
    1967 /*fromYear*/,
    2006 /*toYear*/,
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
    1976 /*fromYear*/,
    1986 /*toYear*/,
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
    1987 /*fromYear*/,
    2006 /*toYear*/,
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
    2007 /*fromYear*/,
    9999 /*toYear*/,
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
    2007 /*fromYear*/,
    9999 /*toYear*/,
    11 /*inMonth*/,
    7 /*onDayOfWeek*/,
    1 /*onDayOfMonth*/,
    8 /*atTimeCode*/,
    kAtcSuffixW /*atTimeModifier*/,
    0 + 4 /*deltaCode*/,
    'S' /*letter*/,
  },

};

static const AtcZonePolicy kPolicyTestUS = {
  kZoneRulesTestUS /*rules*/,
  NULL /* letters */,
  5 /*numRules*/,
  0 /* numLetters */,
};

static const AtcZoneEra kZoneEraTestLos_Angeles[] = {
  //             -8:00    US    P%sT
  {
    &kPolicyTestUS /*zonePolicy*/,
    "P%T" /*format*/,
    -32 /*offsetCode*/,
    0 + 4 /*deltaCode*/,
    10000 /*untilYear*/,
    1 /*untilMonth*/,
    1 /*untilDay*/,
    0 /*untilTimeCode*/,
    kAtcSuffixW /*untilTimeModifier*/,
  },

};

static const AtcZoneInfo kZoneTestLosAngeles = {
  "America/Los_Angeles" /*name*/,
  0xb7f7e8f2 /*zoneId*/,
  &kZoneContext /*zoneContext*/,
  1 /*numEras*/,
  kZoneEraTestLos_Angeles /*eras*/,
};

//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_find_matches_simple) {
  AtcYearMonth start_ym = {2018, 12};
  AtcYearMonth until_ym = {2020, 2};
  const uint8_t kMaxMatches = 4;

  AtcMatchingEra matches[kMaxMatches];
  uint8_t num_matches = atc_processing_find_matches(
      &kZoneAlmostLosAngeles, start_ym, until_ym, matches, kMaxMatches);
  ACU_ASSERT(3 == num_matches);

  {
    AtcDateTuple *sdt = &matches[0].start_dt;
    ACU_ASSERT(sdt->year == 2018);
    ACU_ASSERT(sdt->month == 12);
    ACU_ASSERT(sdt->day == 1);
    ACU_ASSERT(sdt->minutes == 0);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    AtcDateTuple *udt = &matches[0].until_dt;
    ACU_ASSERT(udt->year == 2019);
    ACU_ASSERT(udt->month == 3);
    ACU_ASSERT(udt->day == 10);
    ACU_ASSERT(udt->minutes == 15*8);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const AtcZoneEra *eras =
        (const AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[0].era == &eras[0]);
  }

  {
    AtcDateTuple *sdt = &matches[1].start_dt;
    ACU_ASSERT(sdt->year == 2019);
    ACU_ASSERT(sdt->month == 3);
    ACU_ASSERT(sdt->day == 10);
    ACU_ASSERT(sdt->minutes == 15*8);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    AtcDateTuple *udt = &matches[1].until_dt;
    ACU_ASSERT(udt->year == 2019);
    ACU_ASSERT(udt->month == 11);
    ACU_ASSERT(udt->day == 3);
    ACU_ASSERT(udt->minutes == 15*8);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const AtcZoneEra *eras =
        (const AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[1].era == &eras[1]);
  }

  {
    AtcDateTuple *sdt = &matches[2].start_dt;
    ACU_ASSERT(sdt->year == 2019);
    ACU_ASSERT(sdt->month == 11);
    ACU_ASSERT(sdt->day == 3);
    ACU_ASSERT(sdt->minutes == 15*8);
    ACU_ASSERT(sdt->suffix == kAtcSuffixW);
    //
    AtcDateTuple *udt = &matches[2].until_dt;
    ACU_ASSERT(udt->year == 2020);
    ACU_ASSERT(udt->month == 2);
    ACU_ASSERT(udt->day == 1);
    ACU_ASSERT(udt->minutes == 0);
    ACU_ASSERT(udt->suffix == kAtcSuffixW);
    //
    const AtcZoneEra *eras =
        (const AtcZoneEra *) kZoneAlmostLosAngeles.eras;
    ACU_ASSERT(matches[2].era == &eras[2]);
  }
}

ACU_TEST(test_atc_processing_find_matches_named)
{
  AtcYearMonth start_ym = {2018, 12};
  AtcYearMonth until_ym = {2020, 2};
  const uint8_t kMaxMatches = 4;

  AtcMatchingEra matches[kMaxMatches];
  uint8_t num_matches = atc_processing_find_matches(
      &kZoneTestLosAngeles, start_ym, until_ym, matches, kMaxMatches);
  ACU_ASSERT(1 == num_matches);

  AtcDateTuple *sdt = &matches[0].start_dt;
  ACU_ASSERT(sdt->year == 2018);
  ACU_ASSERT(sdt->month == 12);
  ACU_ASSERT(sdt->day == 1);
  ACU_ASSERT(sdt->minutes == 0);
  ACU_ASSERT(sdt->suffix == kAtcSuffixW);

  AtcDateTuple *udt = &matches[0].until_dt;
  ACU_ASSERT(udt->year == 2020);
  ACU_ASSERT(udt->month == 2);
  ACU_ASSERT(udt->day == 1);
  ACU_ASSERT(udt->minutes == 0);
  ACU_ASSERT(udt->suffix == kAtcSuffixW);

  const AtcZoneEra *eras =
      (const AtcZoneEra *) kZoneTestLosAngeles.eras;
  ACU_ASSERT(matches[0].era == &eras[0]);
}

//---------------------------------------------------------------------------
// Step 2A
//---------------------------------------------------------------------------

ACU_TEST(test_atc_calc_start_day_of_month) {
  // 2018-11, Sun>=1
  AtcMonthDay monthDay = atc_processing_calc_start_day_of_month(
      2018, 11, kAtcIsoWeekdaySunday, 1);
  ACU_ASSERT(11 == monthDay.month);
  ACU_ASSERT(4 == monthDay.day);

  // 2018-11, lastSun
  monthDay = atc_processing_calc_start_day_of_month(
      2018, 11, kAtcIsoWeekdaySunday, 0);
  ACU_ASSERT(11 == monthDay.month);
  ACU_ASSERT(25 == monthDay.day);

  // 2018-11, Sun>=30, should shift to 2018-12-2
  monthDay = atc_processing_calc_start_day_of_month(
      2018, 11, kAtcIsoWeekdaySunday, 30);
  ACU_ASSERT(12 == monthDay.month);
  ACU_ASSERT(2 == monthDay.day);

  // 2018-11, Mon<=7
  monthDay = atc_processing_calc_start_day_of_month(
      2018, 11, kAtcIsoWeekdayMonday, -7);
  ACU_ASSERT(11 == monthDay.month);
  ACU_ASSERT(5 == monthDay.day);

  // 2018-11, Mon<=1, shifts back into October
  monthDay = atc_processing_calc_start_day_of_month(
      2018, 11, kAtcIsoWeekdayMonday, -1);
  ACU_ASSERT(10 == monthDay.month);
  ACU_ASSERT(29 == monthDay.day);

  // 2018-03, Thu>=9
  monthDay = atc_processing_calc_start_day_of_month(
      2018, 3, kAtcIsoWeekdayThursday, 9);
  ACU_ASSERT(3 == monthDay.month);
  ACU_ASSERT(15 == monthDay.day);

  // 2018-03-30 exactly
  monthDay = atc_processing_calc_start_day_of_month(2018, 3, 0, 30);
  ACU_ASSERT(3 == monthDay.month);
  ACU_ASSERT(30 == monthDay.day);
}

ACU_TEST(test_atc_processing_get_transition_time) {
  // Nov Sun>=1
  const AtcZoneRule *rule = &kZoneRulesTestUS[4];

  // Nov 4 2018
  AtcDateTuple dt;
  atc_processing_get_transition_time(2018, rule, &dt);
  ACU_ASSERT(dt.year == 2018);
  ACU_ASSERT(dt.month == 11);
  ACU_ASSERT(dt.day == 4);
  ACU_ASSERT(dt.minutes == 15*8);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // Nov 3 2019
  atc_processing_get_transition_time(2019, rule, &dt);
  ACU_ASSERT(dt.year == 2019);
  ACU_ASSERT(dt.month == 11);
  ACU_ASSERT(dt.day == 3);
  ACU_ASSERT(dt.minutes == 15*8);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);
}

ACU_TEST(test_atc_processing_create_transition_for_year) {
  const AtcMatchingEra match = {
    {2018, 12, 1, 0, kAtcSuffixW},
    {2020, 2, 1, 0, kAtcSuffixW},
    &kZoneEraTestLos_Angeles[0],
    NULL /*prevMatch*/,
    0 /*lastOffsetMinutes*/,
    0 /*lastDeltaMinutes*/
  };

  // Nov Sun>=1
  const AtcZoneRule *rule = &kZoneRulesTestUS[4];
  AtcTransition t;
  atc_processing_create_transition_for_year(&t, 2019, rule, &match);
  ACU_ASSERT(t.offset_minutes == -15*32);
  ACU_ASSERT(t.delta_minutes == 0);
  const AtcDateTuple *tt = &t.transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 11);
  ACU_ASSERT(tt->day == 3);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
}

//---------------------------------------------------------------------------
// Step 2B: Pass 1
//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_calc_interior_years)
{
  const uint8_t kMaxInteriorYears = 4;
  int16_t interior_years[kMaxInteriorYears];

  uint8_t num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 1998, 1999, 2000, 2002);
  ACU_ASSERT(0 == num);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 2003, 2005, 2000, 2002);
  ACU_ASSERT(0 == num);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 1998, 2000, 2000, 2002);
  ACU_ASSERT(1 == num);
  ACU_ASSERT(2000 == interior_years[0]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 2002, 2004, 2000, 2002);
  ACU_ASSERT(1 == num);
  ACU_ASSERT(2002 == interior_years[0]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 2001, 2002, 2000, 2002);
  ACU_ASSERT(2 == num);
  ACU_ASSERT(2001 == interior_years[0]);
  ACU_ASSERT(2002 == interior_years[1]);

  num = atc_processing_calc_interior_years(
      interior_years, kMaxInteriorYears, 1999, 2003, 2000, 2002);
  ACU_ASSERT(3 == num);
  ACU_ASSERT(2000 == interior_years[0]);
  ACU_ASSERT(2001 == interior_years[1]);
  ACU_ASSERT(2002 == interior_years[2]);
}

ACU_TEST(test_atc_processing_get_most_recent_prior_year)
{
  int16_t year;

  year = atc_processing_get_most_recent_prior_year(1998, 1999, 2000, 2002);
  ACU_ASSERT(1999 == year);

  year = atc_processing_get_most_recent_prior_year(2003, 2005, 2000, 2002);
  ACU_ASSERT(kAtcInvalidYear == year);

  year = atc_processing_get_most_recent_prior_year(1998, 2000, 2000, 2002);
  ACU_ASSERT(1999 == year);

  year = atc_processing_get_most_recent_prior_year(2002, 2004, 2000, 2002);
  ACU_ASSERT(kAtcInvalidYear == year);

  year = atc_processing_get_most_recent_prior_year(2001, 2002, 2000, 2002);
  ACU_ASSERT(kAtcInvalidYear == year);

  year = atc_processing_get_most_recent_prior_year(1999, 2003, 2000, 2002);
  ACU_ASSERT(1999 == year);
}

ACU_TEST(test_atc_processing_find_candidate_transitions) {
  AtcMatchingEra match = {
    {2018, 12, 1, 0, kAtcSuffixW},
    {2020, 2, 1, 0, kAtcSuffixW},
    &kZoneEraTestLos_Angeles[0],
    NULL /*prevMatch*/,
    0 /*lastOffsetMinutes*/,
    0 /*lastDeltaMinutes*/
  };

  // Reserve storage for the Transitions
  AtcTransitionStorage storage;
  atc_transition_storage_init(&storage);

  // Verify compareTransitionToMatchFuzzy() elminates various transitions
  // to get down to 5:
  //    * 2018 Mar Sun>=8 (11)
  //    * 2019 Nov Sun>=1 (4)
  //    * 2019 Mar Sun>=8 (10)
  //    * 2019 Nov Sun>=1 (3)
  //    * 2020 Mar Sun>=8 (8)
  atc_transition_storage_reset_candidate_pool(&storage);
  atc_processing_find_candidate_transitions(&storage, &match);
  AtcTransition **end =
      atc_transition_storage_get_candidate_pool_end(&storage);
  AtcTransition **begin =
      atc_transition_storage_get_candidate_pool_begin(&storage);
  ACU_ASSERT(5 == (int) (end - begin));

  AtcTransition **t = &storage.transitions[storage.index_candidate];
  AtcDateTuple *tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2018);
  ACU_ASSERT(tt->month == 3);
  ACU_ASSERT(tt->day == 11);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2018);
  ACU_ASSERT(tt->month == 11);
  ACU_ASSERT(tt->day == 4);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 3);
  ACU_ASSERT(tt->day == 10);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 11);
  ACU_ASSERT(tt->day == 3);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2020);
  ACU_ASSERT(tt->month == 3);
  ACU_ASSERT(tt->day == 8);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
}

//---------------------------------------------------------------------------
// Step 2B: Pass 3
//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_process_transition_match_status)
{
  // UNTIL = 2002-01-02T03:00
  const AtcZoneEra ERA = {
      NULL /*zonePolicy*/,
      "" /*format*/,
      0 /*offsetCode*/,
      0 /*deltaCode*/,
      2002 /*untilYear*/,
      1 /*untilMonth*/,
      2 /*untilDay*/,
      12 /*untilTimeCode*/,
      kAtcSuffixW
  };

  // [2000-01-01, 2001-01-01)
  const AtcMatchingEra match = {
    {2000, 1, 1, 0, kAtcSuffixW} /*startDateTime*/,
    {2001, 1, 1, 0, kAtcSuffixW} /*untilDateTime*/,
    &ERA /*era*/,
    NULL /*prevMatch*/,
    0 /*lastOffsetMinutes*/,
    0 /*lastDeltaMinutes*/
  };

  // This transition occurs before the match, so prior should be filled.
  // transitionTime = 1999-12-31
  AtcTransition transition0 = {
    &match /*match*/,
    NULL /*rule*/,
    {1999, 12, 31, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0, 0, 0, {0}, {0}, {0}
  };

  // This occurs at exactly match.startDateTime, so should replace the prior.
  // transitionTime = 2000-01-01
  AtcTransition transition1 = {
    &match /*match*/,
    NULL /*rule*/,
    {2000, 1, 1, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0, 0, 0, {0}, {0}, {0}
  };

  // An interior transition. Prior should not change.
  // transitionTime = 2000-01-02
  AtcTransition transition2 = {
    &match /*match*/,
    NULL /*rule*/,
    {2000, 1, 2, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0, 0, 0, {0}, {0}, {0}
  };

  // Occurs after match.untilDateTime, so should be rejected.
  // transitionTime = 2001-01-02
  AtcTransition transition3 = {
    &match /*match*/,
    NULL /*rule*/,
    {2001, 1, 2, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0, 0, 0, {0}, {0}, {0}
  };

  AtcTransition *transitions[] = {
    &transition0,
    &transition1,
    &transition2,
    &transition3,
  };

  // Populate the transitionTimeS and transitionTimeU fields.
  AtcTransition *prior = NULL;
  atc_transition_fix_times(&transitions[0], &transitions[4]);

  atc_processing_process_transition_match_status(&transition0, &prior);
  ACU_ASSERT(kAtcMatchStatusPrior == transition0.match_status);
  ACU_ASSERT(prior == &transition0);

  atc_processing_process_transition_match_status(&transition1, &prior);
  ACU_ASSERT(kAtcMatchStatusExactMatch == transition1.match_status);
  ACU_ASSERT(prior == &transition1);

  atc_processing_process_transition_match_status(&transition2, &prior);
  ACU_ASSERT(kAtcMatchStatusWithinMatch == transition2.match_status);
  ACU_ASSERT(prior == &transition1);

  atc_processing_process_transition_match_status(&transition3, &prior);
  ACU_ASSERT(kAtcMatchStatusFarFuture == transition3.match_status);
  ACU_ASSERT(prior == &transition1);
}

//---------------------------------------------------------------------------
// Step 2B
//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_create_transitions_from_named_match)
{
  AtcMatchingEra match = {
    {2018, 12, 1, 0, kAtcSuffixW},
    {2020, 2, 1, 0, kAtcSuffixW},
    &kZoneEraTestLos_Angeles[0],
    NULL /*prevMatch*/,
    0 /*lastOffsetMinutes*/,
    0 /*lastDeltaMinutes*/
  };

  // Reserve storage for the Transitions
  AtcTransitionStorage storage;
  atc_transition_storage_init(&storage);

  atc_processing_create_transitions_from_named_match(&storage, &match);
  AtcTransition **end =
      atc_transition_storage_get_active_pool_end(&storage);
  AtcTransition **begin =
      atc_transition_storage_get_active_pool_begin(&storage);
  ACU_ASSERT(3 == (int) (end - begin));
  //
  AtcTransition **t = &storage.transitions[0];
  AtcDateTuple *tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2018);
  ACU_ASSERT(tt->month == 12);
  ACU_ASSERT(tt->day == 1);
  ACU_ASSERT(tt->minutes == 0);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 3);
  ACU_ASSERT(tt->day == 10);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  //
  tt = &(*t++)->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 11);
  ACU_ASSERT(tt->day == 3);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
}

//---------------------------------------------------------------------------
// Step 3, Step 4
//---------------------------------------------------------------------------

#if ENABLE_DEBUG >= 1
static void print_date_tuple(const AtcDateTuple *dt)
{
  int16_t ms = dt->minutes;
  int16_t h = ms / 60;
  int16_t m = ms % 60;
  char suffix =
      (dt->suffix == kAtcSuffixW ? 'w' :
      (dt->suffix == kAtcSuffixS ? 's' :
      (dt->suffix == kAtcSuffixU ? 'u' : '?')));
  printf("%04d-%02d-%02d %02d:%02d%c",
      dt->year,
      dt->month,
      dt->day,
      h,
      m,
      suffix);
}

static void print_transition(const AtcTransition *t)
{
  printf("tt=");
  print_date_tuple(&t->transition_time);
  printf(";tts=");
  print_date_tuple(&t->transition_time_s);
  printf(";ttu=");
  print_date_tuple(&t->transition_time_u);
  printf(";offset=%d", t->offset_minutes);
  printf(";delta=%d", t->delta_minutes);
  printf("\n");
}
#endif

ACU_TEST(test_fix_transition_times_generate_start_until_times)
{
  // Create 3 matches for the AlmostLosAngeles test zone.
  AtcYearMonth start_ym = {2018, 12};
  AtcYearMonth until_ym = {2020, 2};
  const uint8_t kMaxMatches = 4;
  AtcMatchingEra matches[kMaxMatches];
  uint8_t num_matches = atc_processing_find_matches(
      &kZoneAlmostLosAngeles, start_ym, until_ym, matches, kMaxMatches);
  ACU_ASSERT(3 == num_matches);

  // Create a custom template instantiation to use a different SIZE than the
  // pre-defined typedef in ExtendedZoneProcess::TransitionStorage.
  AtcTransitionStorage storage;
  atc_transition_storage_init(&storage);

  // Create 3 Transitions corresponding to the matches.
  // Implements ExtendedZoneProcessor::createTransitionsFromSimpleMatch().
  AtcTransition *transition1 =
      atc_transition_storage_get_free_agent(&storage);
  atc_processing_create_transition_for_year(
      transition1, 0 /*year, not used*/, NULL /*rule*/, &matches[0]);
  transition1->match_status = kAtcMatchStatusExactMatch; // synthetic example
  atc_transition_storage_add_free_agent_to_candidate_pool(&storage);

  AtcTransition *transition2 =
      atc_transition_storage_get_free_agent(&storage);
  atc_processing_create_transition_for_year(
      transition2, 0 /*year, not used*/, NULL /*rule*/, &matches[1]);
  transition2->match_status = kAtcMatchStatusWithinMatch; // synthetic example
  atc_transition_storage_add_free_agent_to_candidate_pool(&storage);

  AtcTransition *transition3 =
      atc_transition_storage_get_free_agent(&storage);
  atc_processing_create_transition_for_year(
      transition3, 0 /*year, not used*/, NULL /*rule*/, &matches[2]);
  transition3->match_status = kAtcMatchStatusWithinMatch; // synthetic example
  atc_transition_storage_add_free_agent_to_candidate_pool(&storage);

  // Move actives to Active pool.
  atc_transition_storage_add_active_candidates_to_active_pool(&storage);
  AtcTransition **begin =
      atc_transition_storage_get_active_pool_begin(&storage);
  AtcTransition **end =
      atc_transition_storage_get_active_pool_end(&storage);
  ACU_ASSERT(3 == (int) (end - begin));
  ACU_ASSERT(begin[0] == transition1);
  ACU_ASSERT(begin[1] == transition2);
  ACU_ASSERT(begin[2] == transition3);
#if ENABLE_DEBUG >= 1
  print_transition(transition1);
  print_transition(transition2);
  print_transition(transition3);
#endif

  // Chain the transitions.
  atc_transition_fix_times(begin, end);

#if ENABLE_DEBUG >= 1
  print_transition(transition1);
  print_transition(transition2);
  print_transition(transition3);
#endif

  // Verify. The first Transition is extended to -infinity.
  AtcDateTuple *tt = &transition1->transition_time;
  ACU_ASSERT(tt->year == 2018);
  ACU_ASSERT(tt->month == 12);
  ACU_ASSERT(tt->day == 1);
  ACU_ASSERT(tt->minutes == 0);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  AtcDateTuple *tts = &transition1->transition_time_s;
  ACU_ASSERT(tts->year == 2018);
  ACU_ASSERT(tts->month == 12);
  ACU_ASSERT(tts->day == 1);
  ACU_ASSERT(tts->minutes == 0);
  ACU_ASSERT(tts->suffix == kAtcSuffixS);
  AtcDateTuple *ttu = &transition1->transition_time_u;
  ACU_ASSERT(ttu->year == 2018);
  ACU_ASSERT(ttu->month == 12);
  ACU_ASSERT(ttu->day == 1);
  ACU_ASSERT(ttu->minutes == 15*32);
  ACU_ASSERT(ttu->suffix == kAtcSuffixU);

  // Second transition uses the UTC offset of the first.
  tt = &transition2->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 3);
  ACU_ASSERT(tt->day == 10);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  tts = &transition2->transition_time_s;
  ACU_ASSERT(tts->year == 2019);
  ACU_ASSERT(tts->month == 3);
  ACU_ASSERT(tts->day == 10);
  ACU_ASSERT(tts->minutes == 15*8);
  ACU_ASSERT(tts->suffix == kAtcSuffixS);
  ttu = &transition2->transition_time_u;
  ACU_ASSERT(ttu->year == 2019);
  ACU_ASSERT(ttu->month == 3);
  ACU_ASSERT(ttu->day == 10);
  ACU_ASSERT(ttu->minutes == 15*40);
  ACU_ASSERT(ttu->suffix == kAtcSuffixU);

  // Third transition uses the UTC offset of the second.
  tt = &transition3->transition_time;
  ACU_ASSERT(tt->year == 2019);
  ACU_ASSERT(tt->month == 11);
  ACU_ASSERT(tt->day == 3);
  ACU_ASSERT(tt->minutes == 15*8);
  ACU_ASSERT(tt->suffix == kAtcSuffixW);
  tts = &transition3->transition_time_s;
  ACU_ASSERT(tts->year == 2019);
  ACU_ASSERT(tts->month == 11);
  ACU_ASSERT(tts->day == 3);
  ACU_ASSERT(tts->minutes == 15*4);
  ACU_ASSERT(tts->suffix == kAtcSuffixS);
  ttu = &transition3->transition_time_u;
  ACU_ASSERT(ttu->year == 2019);
  ACU_ASSERT(ttu->month == 11);
  ACU_ASSERT(ttu->day == 3);
  ACU_ASSERT(ttu->minutes == 15*36);
  ACU_ASSERT(ttu->suffix == kAtcSuffixU);

  // Generate the startDateTime and untilDateTime of the transitions.
  atc_processing_generate_start_until_times(begin, end);

  // Verify. The first transition startTime should be the same as its
  // transitionTime.
  AtcDateTuple *sdt = &transition1->start_dt;
  ACU_ASSERT(sdt->year == 2018);
  ACU_ASSERT(sdt->month == 12);
  ACU_ASSERT(sdt->day == 1);
  ACU_ASSERT(sdt->minutes == 0);
  ACU_ASSERT(sdt->suffix == kAtcSuffixW);
  //
  AtcDateTuple *udt = &transition1->until_dt;
  ACU_ASSERT(udt->year == 2019);
  ACU_ASSERT(udt->month == 3);
  ACU_ASSERT(udt->day == 10);
  ACU_ASSERT(udt->minutes == 15*8);
  ACU_ASSERT(udt->suffix == kAtcSuffixW);
  //
  AtcOffsetDateTime odt = {
    2018, 12, 1, 0, 0, 0, 0 /*fold*/, -8*60 /*offset_minutes*/
  };
  atc_time_t eps = atc_offset_date_time_to_epoch_seconds(&odt);
  ACU_ASSERT(eps == transition1->start_epoch_seconds);

  // Second transition startTime is shifted forward one hour into PDT.
  sdt = &transition2->start_dt;
  ACU_ASSERT(sdt->year == 2019);
  ACU_ASSERT(sdt->month == 3);
  ACU_ASSERT(sdt->day == 10);
  ACU_ASSERT(sdt->minutes == 15*12);
  ACU_ASSERT(sdt->suffix == kAtcSuffixW);
  //
  udt = &transition2->until_dt;
  ACU_ASSERT(udt->year == 2019);
  ACU_ASSERT(udt->month == 11);
  ACU_ASSERT(udt->day == 3);
  ACU_ASSERT(udt->minutes == 15*8);
  ACU_ASSERT(udt->suffix == kAtcSuffixW);
  //
  odt = (AtcOffsetDateTime) {
    2019, 3, 10, 3, 0, 0, 0 /*fold*/, -7*60 /*offset_minutes*/
  };
  eps = atc_offset_date_time_to_epoch_seconds(&odt);
  ACU_ASSERT(eps == transition2->start_epoch_seconds);

  // Third transition startTime is shifted back one hour into PST.
  sdt = &transition3->start_dt;
  ACU_ASSERT(sdt->year == 2019);
  ACU_ASSERT(sdt->month == 11);
  ACU_ASSERT(sdt->day == 3);
  ACU_ASSERT(sdt->minutes == 15*4);
  ACU_ASSERT(sdt->suffix == kAtcSuffixW);
  //
  udt = &transition3->until_dt;
  ACU_ASSERT(udt->year == 2020);
  ACU_ASSERT(udt->month == 2);
  ACU_ASSERT(udt->day == 1);
  ACU_ASSERT(udt->minutes == 0);
  ACU_ASSERT(udt->suffix == kAtcSuffixW);
  //
  odt = (AtcOffsetDateTime) {
    2019, 11, 3, 1, 0, 0, 0 /*fold*/, -8*60 /*offset_minutes*/
  };
  eps = atc_offset_date_time_to_epoch_seconds(&odt);
  ACU_ASSERT(eps == transition3->start_epoch_seconds);
}

//---------------------------------------------------------------------------
// Step 5
//---------------------------------------------------------------------------

ACU_TEST(test_atc_processing_create_abbreviation)
{
  const uint8_t kDstSize = 6;
  char dst[kDstSize];

  // If no '%', deltaMinutes and letter should not matter
  atc_processing_create_abbreviation(dst, kDstSize, "SAST", 0, NULL);
  ACU_ASSERT(strcmp("SAST", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "SAST", 60, "A");
  ACU_ASSERT(strcmp("SAST", dst) == 0);

  // If '%', and letter is (incorrectly) set to '\0', just copy the thing
  atc_processing_create_abbreviation(dst, kDstSize, "SA%ST", 0, NULL);
  ACU_ASSERT(strcmp("SA%ST", dst) == 0);

  // If '%', then replaced with (non-null) letterString.
  atc_processing_create_abbreviation(dst, kDstSize, "P%T", 60, "D");
  ACU_ASSERT(strcmp("PDT", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "P%T", 0, "S");
  ACU_ASSERT(strcmp("PST", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "P%T", 0, "");
  ACU_ASSERT(strcmp("PT", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "%", 60, "CAT");
  ACU_ASSERT(strcmp("CAT", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "%", 0, "WAT");
  ACU_ASSERT(strcmp("WAT", dst) == 0);

  // If '/', then deltaMinutes selects the first or second component.
  atc_processing_create_abbreviation(dst, kDstSize, "GMT/BST", 0, "");
  ACU_ASSERT(strcmp("GMT", dst) == 0);

  atc_processing_create_abbreviation(dst, kDstSize, "GMT/BST", 60, "");
  ACU_ASSERT(strcmp("BST", dst) == 0);

  // test truncation to kDstSize
  atc_processing_create_abbreviation(dst, kDstSize, "P%T3456", 60, "DD");
  ACU_ASSERT(strcmp("PDDT3", dst) == 0);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_compare_era_to_year_month);
  ACU_RUN_TEST(test_atc_compare_era_to_year_month_equal);
  ACU_RUN_TEST(test_atc_create_matching_era);
  ACU_RUN_TEST(test_atc_processing_find_matches_simple);
  ACU_RUN_TEST(test_atc_processing_find_matches_named);
  ACU_RUN_TEST(test_atc_calc_start_day_of_month);
  ACU_RUN_TEST(test_atc_processing_get_transition_time);
  ACU_RUN_TEST(test_atc_processing_create_transition_for_year);
  ACU_RUN_TEST(test_atc_processing_calc_interior_years);
  ACU_RUN_TEST(test_atc_processing_get_most_recent_prior_year);
  ACU_RUN_TEST(test_atc_processing_find_candidate_transitions);
  ACU_RUN_TEST(test_atc_processing_process_transition_match_status);
  ACU_RUN_TEST(test_atc_processing_create_transitions_from_named_match);
  ACU_RUN_TEST(test_fix_transition_times_generate_start_until_times);
  ACU_RUN_TEST(test_atc_processing_create_abbreviation);
  ACU_SUMMARY();
}
