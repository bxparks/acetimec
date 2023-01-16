#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_atc_local_date_time_to_epoch_seconds)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcLocalDateTime ldt = {1931, 12, 13, 20, 45, 53};
  int32_t seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  ldt = (AtcLocalDateTime) {2000, 1, 1, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 0);

  ldt = (AtcLocalDateTime) {2000, 1, 2, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400);

  ldt = (AtcLocalDateTime) {2000, 2, 29, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400 * 59);

  ldt = (AtcLocalDateTime) {2018, 1, 1, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400 * 6575);

  ldt = (AtcLocalDateTime) {2038, 1, 19, 3, 14, 7};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 1200798847);

  ldt = (AtcLocalDateTime) {2068, 1, 19, 3, 14, 7};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == INT32_MAX);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_atc_local_date_time_from_epoch_seconds)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2000);

  AtcLocalDateTime ldt;
  int8_t err;

  err = atc_local_date_time_from_epoch_seconds(&ldt, INT32_MIN + 1);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 1931);
  ACU_ASSERT(ldt.month == 12);
  ACU_ASSERT(ldt.day == 13);
  ACU_ASSERT(ldt.hour == 20);
  ACU_ASSERT(ldt.minute == 45);
  ACU_ASSERT(ldt.second == 53);

  err = atc_local_date_time_from_epoch_seconds(&ldt, 0);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 2000);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 1);
  ACU_ASSERT(ldt.hour == 0);
  ACU_ASSERT(ldt.minute == 0);
  ACU_ASSERT(ldt.second == 0);

  err = atc_local_date_time_from_epoch_seconds(&ldt, 10958 * 86400 - 1);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 2029);
  ACU_ASSERT(ldt.month == 12);
  ACU_ASSERT(ldt.day == 31);
  ACU_ASSERT(ldt.hour == 23);
  ACU_ASSERT(ldt.minute == 59);
  ACU_ASSERT(ldt.second == 59);

  err = atc_local_date_time_from_epoch_seconds(&ldt, INT32_MAX - 1);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 2068);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 19);
  ACU_ASSERT(ldt.hour == 3);
  ACU_ASSERT(ldt.minute == 14);
  ACU_ASSERT(ldt.second == 6);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_local_date_time_to_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcLocalDateTime ldt = {1981, 12, 13, 20, 45, 53};
  int32_t seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == INT32_MIN + 1);

  ldt = (AtcLocalDateTime) {2050, 1, 1, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 0);

  ldt = (AtcLocalDateTime) {2050, 1, 2, 0, 0, 0};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == 86400);

  ldt = (AtcLocalDateTime) {2118, 1, 20, 3, 14, 7};
  seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  ACU_ASSERT(seconds == INT32_MAX);

  atc_set_current_epoch_year(saved_epoch_year);
}

ACU_TEST(test_atc_local_date_time_from_epoch_seconds_epoch2050)
{
  int16_t saved_epoch_year = atc_get_current_epoch_year();
  atc_set_current_epoch_year(2050);

  AtcLocalDateTime ldt;
  int8_t err;

  err = atc_local_date_time_from_epoch_seconds(&ldt, INT32_MIN + 1);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 1981);
  ACU_ASSERT(ldt.month == 12);
  ACU_ASSERT(ldt.day == 13);
  ACU_ASSERT(ldt.hour == 20);
  ACU_ASSERT(ldt.minute == 45);
  ACU_ASSERT(ldt.second == 53);

  err = atc_local_date_time_from_epoch_seconds(&ldt, 0);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 2050);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 1);
  ACU_ASSERT(ldt.hour == 0);
  ACU_ASSERT(ldt.minute == 0);
  ACU_ASSERT(ldt.second == 0);

  err = atc_local_date_time_from_epoch_seconds(&ldt, INT32_MAX - 1);
  ACU_ASSERT(err == kAtcErrOk);
  ACU_ASSERT(ldt.year == 2118);
  ACU_ASSERT(ldt.month == 1);
  ACU_ASSERT(ldt.day == 20);
  ACU_ASSERT(ldt.hour == 3);
  ACU_ASSERT(ldt.minute == 14);
  ACU_ASSERT(ldt.second == 6);

  atc_set_current_epoch_year(saved_epoch_year);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_local_date_time_to_epoch_seconds);
  ACU_RUN_TEST(test_atc_local_date_time_from_epoch_seconds);
  ACU_RUN_TEST(test_atc_local_date_time_to_epoch_seconds_epoch2050);
  ACU_RUN_TEST(test_atc_local_date_time_from_epoch_seconds_epoch2050);
  ACU_SUMMARY();
}
