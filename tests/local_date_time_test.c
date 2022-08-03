#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_atc_local_date_time_to_epoch_seconds)
{
  struct AtcLocalDateTime ldt = {1931, 12, 13, 20, 45, 53};
  int32_t seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  ldt = (struct AtcLocalDateTime) {2000, 1, 1, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 0);

  ldt = (struct AtcLocalDateTime) {2000, 1, 2, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400);

  ldt = (struct AtcLocalDateTime) {2000, 2, 29, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400 * 59);

  ldt = (struct AtcLocalDateTime) {2018, 1, 1, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400 * 6575);

  ldt = (struct AtcLocalDateTime) {2038, 1, 19, 3, 14, 7};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 1200798847);

  ACU_PASS();
}

ACU_TEST(test_atc_local_date_time_from_epoch_seconds_2000)
{
  struct AtcLocalDateTime ldt;
  atc_local_date_time_from_epoch_seconds(0, &ldt);
  ACU_ASSERT(ldt.year == 2000);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 1);
  ACU_ASSERT(ldt.hour == 0);
  ACU_ASSERT(ldt.minute == 0);
  ACU_ASSERT(ldt.second == 0);

  ACU_PASS();
}

ACU_TEST(test_atc_local_date_time_from_epoch_seconds_2029)
{
  struct AtcLocalDateTime ldt;
  atc_local_date_time_from_epoch_seconds(10958 * 86400 - 1, &ldt);
  ACU_ASSERT(ldt.year == 2029);
  ACU_ASSERT(ldt.month == 12);
  ACU_ASSERT(ldt.day == 31);
  ACU_ASSERT(ldt.hour == 23);
  ACU_ASSERT(ldt.minute == 59);
  ACU_ASSERT(ldt.second == 59);

  ACU_PASS();
}

ACU_TEST(test_atc_local_date_time_from_epoch_seconds_2068)
{
  struct AtcLocalDateTime ldt;
  atc_local_date_time_from_epoch_seconds(INT32_MAX - 1, &ldt);
  ACU_ASSERT(ldt.year == 2068);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 19);
  ACU_ASSERT(ldt.hour == 3);
  ACU_ASSERT(ldt.minute == 14);
  ACU_ASSERT(ldt.second == 6);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_local_date_time_to_epoch_seconds);
  ACU_RUN_TEST(test_atc_local_date_time_from_epoch_seconds_2000);
  ACU_RUN_TEST(test_atc_local_date_time_from_epoch_seconds_2029);
  ACU_RUN_TEST(test_atc_local_date_time_from_epoch_seconds_2068);
  ACU_SUMMARY();
}
