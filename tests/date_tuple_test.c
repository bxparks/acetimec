#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_atc_date_tuple_compare)
{
  AtcDateTuple a = {2000, 1, 1, 0, kAtcSuffixW};
  AtcDateTuple b = {2000, 1, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &b) == 0);

  AtcDateTuple bb = {2000, 1, 1, 0, kAtcSuffixS};
  ACU_ASSERT(atc_date_tuple_compare(&a, &bb) == 0);

  AtcDateTuple c = {2000, 1, 1, 1, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &c) < 0);

  AtcDateTuple d = {2000, 1, 2, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &d) < 0);

  AtcDateTuple e = {2000, 2, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &e) < 0);

  AtcDateTuple f = {2001, 1, 1, 0, kAtcSuffixW};
  ACU_ASSERT(atc_date_tuple_compare(&a, &f) < 0);
}

ACU_TEST(test_atc_date_tuple_subtract)
{
  {
    AtcDateTuple dta = {2000, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00:00
    AtcDateTuple dtb = {2000, 1, 1, 1, kAtcSuffixW}; // 2000-01-01 00:00:01
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT(-1 == diff);
  }

  {
    AtcDateTuple dta = {2000, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00
    AtcDateTuple dtb = {2000, 1, 2, 0, kAtcSuffixW}; // 2000-01-02 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 == diff);
  }

  {
    AtcDateTuple dta = {2000, 1, 1, 0, kAtcSuffixW}; // 2000-01-01 00:00
    AtcDateTuple dtb = {2000, 2, 1, 0, kAtcSuffixW}; // 2000-02-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 31 == diff); // January has 31 days
  }

  {
    AtcDateTuple dta = {2000, 2, 1, 0, kAtcSuffixW}; // 2000-02-01 00:00
    AtcDateTuple dtb = {2000, 3, 1, 0, kAtcSuffixW}; // 2000-03-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 29 == diff); // Feb 2000 is leap, 29 days
  }
}

ACU_TEST(test_atc_date_tuple_subtract_no_overflow)
{
  {
    AtcDateTuple dta = {6000, 1, 1, 0, kAtcSuffixW}; // 6000-01-01 00:00:00
    AtcDateTuple dtb = {6000, 1, 1, 1, kAtcSuffixW}; // 6000-01-01 00:00:01
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT(-1 == diff);
  }

  {
    AtcDateTuple dta = {6000, 1, 1, 0, kAtcSuffixW}; // 6000-01-01 00:00
    AtcDateTuple dtb = {6000, 1, 2, 0, kAtcSuffixW}; // 6000-01-02 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 == diff);
  }

  {
    AtcDateTuple dta = {6000, 1, 1, 0, kAtcSuffixW}; // 6000-01-01 00:00
    AtcDateTuple dtb = {6000, 2, 1, 0, kAtcSuffixW}; // 6000-02-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 31 == diff); // January has 31 days
  }

  {
    AtcDateTuple dta = {6000, 2, 1, 0, kAtcSuffixW}; // 6000-02-01 00:00
    AtcDateTuple dtb = {6000, 3, 1, 0, kAtcSuffixW}; // 6000-03-01 00:00
    atc_time_t diff = atc_date_tuple_subtract(&dta, &dtb);
    ACU_ASSERT((int32_t) -86400 * 29 == diff); // Feb 6000 is leap, 29 days
  }
}

ACU_TEST(test_atc_date_tuple_normalize)
{
  // 00:00
  AtcDateTuple dt = {2000, 1, 1, 0, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 2000);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 1);
  ACU_ASSERT(dt.seconds == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 23:45
  dt = (AtcDateTuple) {2000, 1, 1, 23*3600 + 45*60, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 2000);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 1);
  ACU_ASSERT(dt.seconds == 23*3600 + 45*60);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 24:00
  dt = (AtcDateTuple) {2000, 1, 1, 24*3600, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 2000);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 2);
  ACU_ASSERT(dt.seconds == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // 24:15
  dt = (AtcDateTuple) {2000, 1, 1, 24*3600 + 15*60, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 2000);
  ACU_ASSERT(dt.month == 1);
  ACU_ASSERT(dt.day == 2);
  ACU_ASSERT(dt.seconds == 15*60);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // -24:00
  dt = (AtcDateTuple) {2000, 1, 1, -24*3600, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 1999);
  ACU_ASSERT(dt.month == 12);
  ACU_ASSERT(dt.day == 31);
  ACU_ASSERT(dt.seconds == 0);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);

  // -24:15
  dt = (AtcDateTuple) {2000, 1, 1, -24*3600 - 15*60, kAtcSuffixW};
  atc_date_tuple_normalize(&dt);
  ACU_ASSERT(dt.year == 1999);
  ACU_ASSERT(dt.month == 12);
  ACU_ASSERT(dt.day == 31);
  ACU_ASSERT(dt.seconds == -15*60);
  ACU_ASSERT(dt.suffix == kAtcSuffixW);
}

ACU_TEST(test_atc_date_tuple_expand)
{
  AtcDateTuple ttw;
  AtcDateTuple tts;
  AtcDateTuple ttu;

  int32_t offset_seconds = 2*3600;
  int32_t delta_seconds = 1*3600;

  AtcDateTuple tt = {2000, 1, 30, 4*3600, kAtcSuffixW}; // 04:00
  atc_date_tuple_expand(
      &tt, offset_seconds, delta_seconds, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year == 2000);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.seconds == 4*3600);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year == 2000);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.seconds == 3*3600);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year == 2000);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.seconds == 1*3600);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (AtcDateTuple) {2000, 1, 30, 3*3600, kAtcSuffixS};
  atc_date_tuple_expand(
      &tt, offset_seconds, delta_seconds, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year == 2000);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.seconds == 4*3600);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year == 2000);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.seconds == 3*3600);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year == 2000);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.seconds == 1*3600);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (AtcDateTuple) {2000, 1, 30, 1*3600, kAtcSuffixU};
  atc_date_tuple_expand(
      &tt, offset_seconds, delta_seconds, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year == 2000);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.seconds == 4*3600);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year == 2000);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.seconds == 3*3600);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year == 2000);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.seconds == 1*3600);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);
}

ACU_TEST(test_atc_date_tuple_compare_fuzzy) {
  ACU_ASSERT(kAtcComparePrior == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2000, 10, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  ACU_ASSERT(kAtcCompareWithinMatch == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2000, 11, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  ACU_ASSERT(kAtcCompareWithinMatch == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  ACU_ASSERT(kAtcCompareWithinMatch == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2002, 2, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  ACU_ASSERT(kAtcCompareWithinMatch == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2002, 3, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  ACU_ASSERT(kAtcCompareFarFuture == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){2002, 4, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));

  // Verify dates whose delta months is greater than 32767. In
  // other words, delta years is greater than 2730.
  ACU_ASSERT(kAtcCompareFarFuture == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){5000, 4, 1, 1, 0},
      &(AtcDateTuple){2000, 12, 1, 1, 0},
      &(AtcDateTuple){2002, 2, 1, 1, 0}));
  ACU_ASSERT(kAtcComparePrior == atc_date_tuple_compare_fuzzy(
      &(AtcDateTuple){1000, 4, 1, 1, 0},
      &(AtcDateTuple){4000, 12, 1, 1, 0},
      &(AtcDateTuple){4002, 2, 1, 1, 0}));
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_date_tuple_compare);
  ACU_RUN_TEST(test_atc_date_tuple_subtract);
  ACU_RUN_TEST(test_atc_date_tuple_subtract_no_overflow);
  ACU_RUN_TEST(test_atc_date_tuple_normalize);
  ACU_RUN_TEST(test_atc_date_tuple_expand);
  ACU_RUN_TEST(test_atc_date_tuple_compare_fuzzy);
  ACU_SUMMARY();
}
