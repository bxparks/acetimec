#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_offset_date_time_to_epoch_seconds)
{
  // smallest valid offset_date_time
  struct AtcOffsetDateTime odt = {
    1931, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MIN + 1 == atc_offset_date_time_to_epoch_seconds(&odt));

  // offset_date_time at exactly epoch_seconds==0
  odt = (struct AtcOffsetDateTime) {
    2000, 1, 1, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(0 == atc_offset_date_time_to_epoch_seconds(&odt));

  // one day later
  odt = (struct AtcOffsetDateTime) {
    2000, 1, 2, 0, 0, 0, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(86400 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest value using Unix Epoch
  odt = (struct AtcOffsetDateTime) {
    2038, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(1200798847 == atc_offset_date_time_to_epoch_seconds(&odt));

  // largest valid offset_date_time given 32-bit epoch_seconds
  odt = (struct AtcOffsetDateTime) {
    2068, 1, 19, 3, 14, 7, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(INT32_MAX == atc_offset_date_time_to_epoch_seconds(&odt));

  ACU_PASS();
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_with_offset)
{
  uint16_t offset_minutes = 1;
  struct AtcOffsetDateTime odt = {
    2000, 1, 1, 0, 0, 0, 0 /*fold*/, offset_minutes /*offset*/
  };
  ACU_ASSERT(offset_minutes * -60
      == atc_offset_date_time_to_epoch_seconds(&odt));

  ACU_PASS();
}

ACU_TEST(test_offset_date_time_to_epoch_seconds_invalid)
{
  struct AtcOffsetDateTime odt = {
    kAtcInvalidYear, 12, 13, 20, 45, 53, 0 /*fold*/, 0 /*offset*/
  };
  ACU_ASSERT(kAtcInvalidEpochSeconds
      == atc_offset_date_time_to_epoch_seconds(&odt));
  
  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_with_offset);
  ACU_RUN_TEST(test_offset_date_time_to_epoch_seconds_invalid);
  ACU_SUMMARY();
}
