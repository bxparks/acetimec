#include <acunit.h>
#include <acetimec.h>

//---------------------------------------------------------------------------

ACU_TEST(test_atc_date_tuple_compare)
{
  struct AtcDateTuple a = {0, 1, 1, 0, kAtcSuffixW};
  struct AtcDateTuple b = {0, 1, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &b) == 0);

  struct AtcDateTuple bb = {0, 1, 1, 0, kAtcSuffixS};
  ACU_ASSERT(atc_date_tuple_compare(&a, &bb) == 0);

  struct AtcDateTuple c = {0, 1, 1, 1, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &c) < 0);

  struct AtcDateTuple d = {0, 1, 2, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &d) < 0);

  struct AtcDateTuple e = {0, 2, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &e) < 0);

  struct AtcDateTuple f = {1, 1, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &f) < 0);
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
}

ACU_TEST(test_atc_date_tuple_normalize)
{
  // 00:00
  struct AtcDateTuple dt = {0, 1, 1, 0, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == 0);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 1);
  ACU_ASSERT(dt.minutes == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 23:45
  dt = (struct AtcDateTuple) {0, 1, 1, 15*95, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == 0);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 1);
  ACU_ASSERT(dt.minutes == 15*95);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 24:00
  dt = (struct AtcDateTuple) {0, 1, 1, 15*96, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == 0);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 2);
  ACU_ASSERT(dt.minutes == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 24:15
  dt = (struct AtcDateTuple) {0, 1, 1, 15*97, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == 0);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 2);
  ACU_ASSERT(dt.minutes == 15);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // -24:00
  dt = (struct AtcDateTuple) {0, 1, 1, -15*96, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == -1);
  ACU_ASSERT(dt.month == 12);
  ACU_ASSERT(dt.day == 31);
  ACU_ASSERT(dt.minutes == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // -24:15
  dt = (struct AtcDateTuple) {0, 1, 1, -15*97, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year_tiny == -1);
  ACU_ASSERT(dt.month == 12);
  ACU_ASSERT(dt.day == 31);
  ACU_ASSERT(dt.minutes == -15);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);
}

ACU_TEST(test_atc_date_tuple_expand)
{
  struct AtcDateTuple ttw;
  struct AtcDateTuple tts;
  struct AtcDateTuple ttu;

  int16_t offset_minutes = 2*60;
  int16_t delta_minutes = 1*60;

  struct AtcDateTuple tt = {0, 1, 30, 15*16, kAtcSuffixW}; // 04:00
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*16);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*12);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 15*4);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (struct AtcDateTuple) {0, 1, 30, 15*12, kAtcSuffixS};
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*16);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*12);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 15*4);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (struct AtcDateTuple) {0, 1, 30, 15*4, kAtcSuffixU};
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*16);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*12);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 15*4);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_transition_compare_to_match_fuzzy)
{
  const struct AtcMatchingEra match = {
    {0, 1, 1, 0, kAtcSuffixW} /* start_dt */,
    {1, 1, 1, 0, kAtcSuffixW} /* until_dt */,
    NULL /*era*/,
    NULL /*prev_match*/,
    0 /*last_offset_minutes*/,
    0 /*last_delta_minutes*/
  };

  struct AtcTransition transition = {
    &match /*match*/,
    NULL /*rule*/,
    {-1, 11, 1, 0, kAtcSuffixW} /*transition_time*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0 /*start_epoch_seconds*/,
    0 /*offset_minutes*/,
    0 /*delta_minutes*/,
    {0} /*abbrev*/,
    {0} /*letter_buf*/,
    {0} /*match_status*/
  };
  uint8_t status = atc_transition_compare_to_match_fuzzy(&transition, &match);
  ACU_ASSERT(status == kAtcMatchStatusPrior);

  transition = (struct AtcTransition) {
    &match /*match*/,
    NULL /*rule*/,
    {-1, 12, 1, 0, kAtcSuffixW} /*transition_time*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0 /*start_epoch_seconds*/,
    0 /*offset_minutes*/,
    0 /*delta_minutes*/,
    {0} /*abbrev*/,
    {0} /*letter_buf*/,
    {0} /*match_status*/
  };
  status = atc_transition_compare_to_match_fuzzy(&transition, &match);
  ACU_ASSERT(status == kAtcMatchStatusWithinMatch);

  transition = (struct AtcTransition) {
    &match /*match*/,
    NULL /*rule*/,
    {0, 1, 1, 0, kAtcSuffixW} /*transition_time*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0 /*start_epoch_seconds*/,
    0 /*offset_minutes*/,
    0 /*delta_minutes*/,
    {0} /*abbrev*/,
    {0} /*letter_buf*/,
    {0} /*match_status*/
  };
  status = atc_transition_compare_to_match_fuzzy(&transition, &match);
  ACU_ASSERT(status == kAtcMatchStatusWithinMatch);

  transition = (struct AtcTransition) {
    &match /*match*/,
    NULL /*rule*/,
    {1, 1, 1, 0, kAtcSuffixW} /*transition_time*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0 /*start_epoch_seconds*/,
    0 /*offset_minutes*/,
    0 /*delta_minutes*/,
    {0} /*abbrev*/,
    {0} /*letter_buf*/,
    {0} /*match_status*/
  };
  status = atc_transition_compare_to_match_fuzzy(&transition, &match);
  ACU_ASSERT(status == kAtcMatchStatusWithinMatch);

  transition = (struct AtcTransition) {
    &match /*match*/,
    NULL /*rule*/,
    {1, 3, 1, 0, kAtcSuffixW} /*transition_time*/,
    {{0, 0, 0, 0, 0}} /*start_dt*/,
    {{0, 0, 0, 0, 0}} /*until_dt*/,
    0 /*start_epoch_seconds*/,
    0 /*offset_minutes*/,
    0 /*delta_minutes*/,
    {0} /*abbrev*/,
    {0} /*letter_buf*/,
    {0} /*match_status*/
  };
  status = atc_transition_compare_to_match_fuzzy(&transition, &match);
  ACU_ASSERT(status == kAtcMatchStatusFarFuture);
}

ACU_TEST(test_atc_transition_compare_to_match)
{
  // UNTIL = 2002-01-02T03:00
  const struct AtcZoneEra ERA = {
      NULL /*zonePolicy*/,
      "" /*format*/,
      0 /*offsetCode*/,
      0 /*deltaCode*/,
      2 /*untilYearTiny*/,
      1 /*untilMonth*/,
      2 /*untilDay*/,
      12 /*untilTimeCode*/,
      kAtcSuffixW
  };

  // MatchingEra=[2000-01-01, 2001-01-01)
  const struct AtcMatchingEra match = {
    {0, 1, 1, 0, kAtcSuffixW} /*startDateTime*/,
    {1, 1, 1, 0, kAtcSuffixW} /*untilDateTime*/,
    &ERA /*era*/,
    NULL /*prevMatch*/,
    0 /*lastOffsetMinutes*/,
    0 /*lastDeltaMinutes*/
  };

  // transitionTime = 1999-12-31
  struct AtcTransition transition0 = {
    &match /*match*/,
    NULL /*rule*/,
    {-1, 12, 31, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0}},
    0, 0, 0, {0}, {0},
    {0}
  };

  // transitionTime = 2000-01-01
  struct AtcTransition transition1 = {
    &match /*match*/,
    NULL /*rule*/,
    {0, 1, 1, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0}},
    0, 0, 0, {0}, {0},
    {0}
  };

  // transitionTime = 2000-01-02
  struct AtcTransition transition2 = {
    &match /*match*/,
    NULL /*rule*/,
    {0, 1, 2, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0}},
    0, 0, 0, {0}, {0},
    {0}
  };

  // transitionTime = 2001-02-03
  struct AtcTransition transition3 = {
    &match /*match*/,
    NULL /*rule*/,
    {1, 2, 3, 0, kAtcSuffixW} /*transitionTime*/,
    {{0, 0, 0, 0, 0}},
    {{0, 0, 0, 0, 0}},
    0, 0, 0, {0}, {0},
    {0}
  };

  struct AtcTransition *transitions[] = {
    &transition0,
    &transition1,
    &transition2,
    &transition3,
  };

  // Populate the transitionTimeS and transitionTimeU fields.
  atc_transition_fix_times(&transitions[0], &transitions[4]);

  uint8_t status = atc_transition_compare_to_match(&transition0, &match);
  ACU_ASSERT(status == kAtcMatchStatusPrior);

  status = atc_transition_compare_to_match(&transition1, &match);
  ACU_ASSERT(status == kAtcMatchStatusExactMatch);

  status = atc_transition_compare_to_match(&transition2, &match);
  ACU_ASSERT(status == kAtcMatchStatusWithinMatch);

  status = atc_transition_compare_to_match(&transition3, &match);
  ACU_ASSERT(status == kAtcMatchStatusFarFuture);
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_transition_storage_add_free_agent_to_active_pool) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  struct AtcTransition *free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[0]);
  atc_transition_storage_add_free_agent_to_active_pool(&ts);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_prior);
  ACU_ASSERT(1 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[1]);
  atc_transition_storage_add_free_agent_to_active_pool(&ts);
  ACU_ASSERT(2 == ts.index_candidate);
  ACU_ASSERT(2 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[2]);
  atc_transition_storage_add_free_agent_to_active_pool(&ts);
  ACU_ASSERT(3 == ts.index_candidate);
  ACU_ASSERT(3 == ts.index_prior);
  ACU_ASSERT(3 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[3]);
  atc_transition_storage_add_free_agent_to_active_pool(&ts);
  ACU_ASSERT(4 == ts.index_candidate);
  ACU_ASSERT(4 == ts.index_prior);
  ACU_ASSERT(4 == ts.index_free);
}

ACU_TEST(test_atc_transition_storage_add_free_agent_to_candidate_pool) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  // Add the first one to active
  struct AtcTransition *free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[0]);
  atc_transition_storage_add_free_agent_to_active_pool(&ts);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_prior);
  ACU_ASSERT(1 == ts.index_free);

  // Add subsequent ones to candidate pool
  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[1]);
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[2]);
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_prior);
  ACU_ASSERT(3 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(free_agent == &ts.transition_pool[3]);
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_prior);
  ACU_ASSERT(4 == ts.index_free);
}

ACU_TEST(test_atc_transition_storage_reserve_prior) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  struct AtcTransition** prior = atc_transition_storage_reserve_prior(&ts);
  ACU_ASSERT(prior == &ts.transitions[0]);
  ACU_ASSERT(0 == ts.index_prior);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_free);
}

ACU_TEST(test_atc_transition_storage_add_prior_to_candidate_pool) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  struct AtcTransition** prior = atc_transition_storage_reserve_prior(&ts);
  ACU_ASSERT(prior == &ts.transitions[0]);
  ACU_ASSERT(0 == ts.index_prior);
  ACU_ASSERT(1 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_free);

  atc_transition_storage_add_prior_to_candidate_pool(&ts);
  ACU_ASSERT(0 == ts.index_prior);
  ACU_ASSERT(0 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_free);
}

ACU_TEST(test_atc_transition_storage_set_free_agent_as_prior_if_valid) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  // Initial prior
  struct AtcTransition** prior_reservation =
      atc_transition_storage_reserve_prior(&ts);
  (*prior_reservation)->is_valid_prior = false;
  (*prior_reservation)->transition_time = (struct AtcDateTuple)
      {2, 3, 4, 5, kAtcSuffixW};

  // Candiate prior.
  struct AtcTransition* free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->is_valid_prior = true;
  free_agent->transition_time = (struct AtcDateTuple) {2, 3, 4, 0, kAtcSuffixW};

  // Should swap because prior->is_valid_prior is false.
  atc_transition_storage_set_free_agent_as_prior_if_valid(&ts);

  // Verify that the two have been swapped.
  struct AtcTransition* prior = ts.transitions[ts.index_prior];
  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(prior->is_valid_prior == true);
  ACU_ASSERT(free_agent->is_valid_prior == false);
  //
  ACU_ASSERT(prior->transition_time.year_tiny == 2);
  ACU_ASSERT(prior->transition_time.month == 3);
  ACU_ASSERT(prior->transition_time.day == 4);
  ACU_ASSERT(prior->transition_time.minutes == 0);
  ACU_ASSERT(prior->transition_time.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(free_agent->transition_time.year_tiny == 2);
  ACU_ASSERT(free_agent->transition_time.month == 3);
  ACU_ASSERT(free_agent->transition_time.day == 4);
  ACU_ASSERT(free_agent->transition_time.minutes == 5);
  ACU_ASSERT(free_agent->transition_time.suffix == kAtcSuffixW);

  // Another Candidate prior.
  free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->is_valid_prior = true;
  free_agent->transition_time = (struct AtcDateTuple) {2, 3, 4, 6, kAtcSuffixW};

  // Should swap because the transition_time is newer
  atc_transition_storage_set_free_agent_as_prior_if_valid(&ts);

  // Verify that the two have been swapped.
  prior = ts.transitions[ts.index_prior];
  free_agent = atc_transition_storage_get_free_agent(&ts);
  ACU_ASSERT(prior->is_valid_prior == true);
  ACU_ASSERT(free_agent->is_valid_prior == false);
  //
  ACU_ASSERT(prior->transition_time.year_tiny == 2);
  ACU_ASSERT(prior->transition_time.month == 3);
  ACU_ASSERT(prior->transition_time.day == 4);
  ACU_ASSERT(prior->transition_time.minutes == 6);
  ACU_ASSERT(prior->transition_time.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(free_agent->transition_time.year_tiny == 2);
  ACU_ASSERT(free_agent->transition_time.month == 3);
  ACU_ASSERT(free_agent->transition_time.day == 4);
  ACU_ASSERT(free_agent->transition_time.minutes == 0);
  ACU_ASSERT(free_agent->transition_time.suffix == kAtcSuffixW);
}

ACU_TEST(test_atc_transition_storage_add_active_candidates_to_active_pool) {
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  // create Prior to make it interesting
  struct AtcTransition** prior = atc_transition_storage_reserve_prior(&ts);
  (*prior)->transition_time = (struct AtcDateTuple) {-1, 0, 1, 2, kAtcSuffixW};
  (*prior)->match_status = kAtcMatchStatusWithinMatch;

  // Add 3 transitions to Candidate pool, 2 active, 1 inactive.
  struct AtcTransition* free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {0, 1, 2, 3, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusWithinMatch;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {2, 3, 4, 5, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusWithinMatch;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {1, 2, 3, 4, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusFarPast;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);

  // Add prior into the Candidate pool.
  atc_transition_storage_add_prior_to_candidate_pool(&ts);

  // Add the actives to the Active pool.
  atc_transition_storage_add_active_candidates_to_active_pool(&ts);

  // Verify that there are 3 transitions in the Active pool.
  ACU_ASSERT(3 == ts.index_prior);
  ACU_ASSERT(3 == ts.index_candidate);
  ACU_ASSERT(3 == ts.index_free);
  ACU_ASSERT(-1 == ts.transitions[0]->transition_time.year_tiny);
  ACU_ASSERT(0 == ts.transitions[1]->transition_time.year_tiny);
  ACU_ASSERT(2 == ts.transitions[2]->transition_time.year_tiny);
}

ACU_TEST(test_atc_transition_storage_reset_candidate_pool)
{
  struct AtcTransitionStorage ts;
  atc_transition_storage_init(&ts);

  // Add 2 transitions to Candidate pool, 2 active, 1 inactive.
  struct AtcTransition* free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {0, 1, 2, 3, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusWithinMatch;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(0 == ts.index_prior);
  ACU_ASSERT(0 == ts.index_candidate);
  ACU_ASSERT(1 == ts.index_free);

  free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {2, 3, 4, 5, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusWithinMatch;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(0 == ts.index_prior);
  ACU_ASSERT(0 == ts.index_candidate);
  ACU_ASSERT(2 == ts.index_free);

  // Add active candidates to Active pool. Looks like this
  // already does a resetCandidatePool() effectively.
  atc_transition_storage_add_active_candidates_to_active_pool(&ts);
  ACU_ASSERT(2 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_candidate);
  ACU_ASSERT(2 == ts.index_free);

  // This should be a no-op.
  atc_transition_storage_reset_candidate_pool(&ts);
  ACU_ASSERT(2 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_candidate);
  ACU_ASSERT(2 == ts.index_free);

  // Non-active can be added to the candidate pool.
  free_agent = atc_transition_storage_get_free_agent(&ts);
  free_agent->transition_time = (struct AtcDateTuple) {1, 2, 3, 4, kAtcSuffixW};
  free_agent->match_status = kAtcMatchStatusFarPast;
  atc_transition_storage_add_free_agent_to_candidate_pool(&ts);
  ACU_ASSERT(2 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_candidate);
  ACU_ASSERT(3 == ts.index_free);

  // Reset should remove any remaining candidate transitions.
  atc_transition_storage_reset_candidate_pool(&ts);
  ACU_ASSERT(2 == ts.index_prior);
  ACU_ASSERT(2 == ts.index_candidate);
  ACU_ASSERT(2 == ts.index_free);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_atc_date_tuple_compare);
  ACU_RUN_TEST(test_atc_date_tuple_subtract);
  ACU_RUN_TEST(test_atc_date_tuple_normalize);
  ACU_RUN_TEST(test_atc_date_tuple_expand);
  ACU_RUN_TEST(test_atc_transition_compare_to_match_fuzzy);
  ACU_RUN_TEST(test_atc_transition_compare_to_match);
  ACU_RUN_TEST(test_atc_transition_storage_add_free_agent_to_active_pool);
  ACU_RUN_TEST(test_atc_transition_storage_add_free_agent_to_candidate_pool);
  ACU_RUN_TEST(test_atc_transition_storage_reserve_prior);
  ACU_RUN_TEST(test_atc_transition_storage_add_prior_to_candidate_pool);
  ACU_RUN_TEST(test_atc_transition_storage_set_free_agent_as_prior_if_valid);
  ACU_RUN_TEST(
      test_atc_transition_storage_add_active_candidates_to_active_pool);
  ACU_RUN_TEST(test_atc_transition_storage_reset_candidate_pool);

  ACU_SUMMARY();
}
