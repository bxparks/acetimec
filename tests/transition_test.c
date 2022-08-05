#include "acunit.h"
#include <acetimec.h>

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

  ACU_PASS();
}

ACU_TEST(test_atc_date_tuple_expand)
{
  struct AtcDateTuple ttw;
  struct AtcDateTuple tts;
  struct AtcDateTuple ttu;

  int16_t offset_minutes = 2*60;
  int16_t delta_minutes = 1*60;

  struct AtcDateTuple tt = {0, 1, 30, 15*12, kAtcSuffixW}; // 03:00
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*12);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*8);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 0);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (struct AtcDateTuple) {0, 1, 30, 15*8, kAtcSuffixS};
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*12);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*8);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 0);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  tt = (struct AtcDateTuple) {0, 1, 30, 0, kAtcSuffixU};
  atc_date_tuple_expand(
      &tt, offset_minutes, delta_minutes, &ttw, &tts, &ttu);
  ACU_ASSERT(ttw.year_tiny == 0);
  ACU_ASSERT(ttw.month == 1);
  ACU_ASSERT(ttw.day == 30);
  ACU_ASSERT(ttw.minutes == 15*12);
  ACU_ASSERT(ttw.suffix == kAtcSuffixW);
  //
  ACU_ASSERT(tts.year_tiny == 0);
  ACU_ASSERT(tts.month == 1);
  ACU_ASSERT(tts.day == 30);
  ACU_ASSERT(tts.minutes == 15*8);
  ACU_ASSERT(tts.suffix == kAtcSuffixS);
  //
  ACU_ASSERT(ttu.year_tiny == 0);
  ACU_ASSERT(ttu.month == 1);
  ACU_ASSERT(ttu.day == 30);
  ACU_ASSERT(ttu.minutes == 0);
  ACU_ASSERT(ttu.suffix == kAtcSuffixU);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_date_tuple_compare);
  ACU_RUN_TEST(test_atc_date_tuple_subtract);
  ACU_RUN_TEST(test_atc_date_tuple_normalize);
  ACU_RUN_TEST(test_atc_date_tuple_expand);
  ACU_SUMMARY();
}
