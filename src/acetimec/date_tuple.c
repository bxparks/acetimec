/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#include <stdbool.h>
#include "../zoneinfo/zone_info.h"
#include "../zoneinfo/zone_info_utils.h"
#include "common.h" // atc_time_t
#include "plain_date.h"
#include "date_tuple.h"

int8_t atc_date_tuple_compare(
  const AtcDateTuple *a,
  const AtcDateTuple *b)
{
  if (a->year < b->year) return -1;
  if (a->year > b->year) return 1;
  if (a->month < b->month) return -1;
  if (a->month > b->month) return 1;
  if (a->day < b->day) return -1;
  if (a->day > b->day) return 1;
  if (a->seconds < b->seconds) return -1;
  if (a->seconds > b->seconds) return 1;
  return 0;
}

atc_time_t atc_date_tuple_subtract(
    const AtcDateTuple *a,
    const AtcDateTuple *b)
{
  int32_t da = atc_plain_date_to_epoch_days(a->year, a->month, a->day);
  int32_t db = atc_plain_date_to_epoch_days(b->year, b->month, b->day);

  // Subtract the days, before converting to seconds, to avoid overflowing the
  // int32_t when a.year or b.year is more than 68 years from the
  // atc_current_epoch_year.
  return (da - db) * 86400 + (a->seconds - b->seconds);
}

void atc_date_tuple_expand(
    const AtcDateTuple *tt,
    int32_t offset_seconds,
    int32_t delta_seconds,
    AtcDateTuple *ttw,
    AtcDateTuple *tts,
    AtcDateTuple *ttu)
{

  if (tt->suffix == kAtcSuffixS) {
    *tts = *tt;

    ttu->year = tt->year;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->seconds = tt->seconds - offset_seconds;
    ttu->suffix = kAtcSuffixU;

    ttw->year = tt->year;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->seconds = tt->seconds + delta_seconds;
    ttw->suffix = kAtcSuffixW;
  } else if (tt->suffix == kAtcSuffixU) {
    *ttu = *tt;

    tts->year = tt->year;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->seconds = tt->seconds + offset_seconds;
    tts->suffix = kAtcSuffixS;

    ttw->year = tt->year;
    ttw->month = tt->month;
    ttw->day = tt->day;
    ttw->seconds = tt->seconds + (offset_seconds + delta_seconds);
    ttw->suffix = kAtcSuffixW;
  } else {
    // Explicit set the suffix to 'w' in case it was something else.
    *ttw = *tt;
    ttw->suffix = kAtcSuffixW;

    tts->year = tt->year;
    tts->month = tt->month;
    tts->day = tt->day;
    tts->seconds = tt->seconds - delta_seconds;
    tts->suffix = kAtcSuffixS;

    ttu->year = tt->year;
    ttu->month = tt->month;
    ttu->day = tt->day;
    ttu->seconds = tt->seconds - (delta_seconds + offset_seconds);
    ttu->suffix = kAtcSuffixU;
  }

  atc_date_tuple_normalize(ttw);
  atc_date_tuple_normalize(tts);
  atc_date_tuple_normalize(ttu);
}

void atc_date_tuple_normalize(AtcDateTuple *dt)
{
  const int32_t kOneDayAsSeconds = 60 * 60 * 24;

  if (dt->seconds <= -kOneDayAsSeconds) {
    atc_plain_date_decrement_one_day(&dt->year, &dt->month, &dt->day);
    dt->seconds += kOneDayAsSeconds;
  } else if (kOneDayAsSeconds <= dt->seconds) {
    atc_plain_date_increment_one_day(&dt->year, &dt->month, &dt->day);
    dt->seconds -= kOneDayAsSeconds;
  } else {
    // do nothing
  }
}

uint8_t atc_date_tuple_compare_fuzzy(
    const AtcDateTuple *t,
    const AtcDateTuple *start,
    const AtcDateTuple *until)
{
  // Use int32_t because a delta year of 2730 or greater will exceed
  // the range of an int16_t.
  int32_t t_months = t->year * (int32_t) 12 + t->month;
  int32_t start_months = start->year * (int32_t) 12 + start->month;
  if (t_months < start_months - 1) return kAtcComparePrior;
  int32_t until_months = until->year * 12 + until->month;
  if (until_months + 1 < t_months) return kAtcCompareFarFuture;
  return kAtcCompareWithinMatch;
}
