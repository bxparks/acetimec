#include <stdint.h> // int64_t
#include <acunit.h>
#include <string.h>
#include <acetimec.h>

ACU_TEST(test_atc_convert_to_unix_seconds)
{
  atc_time_t epoch_seconds = 0; // 2050-01-01
  int64_t unix_seconds = atc_convert_to_unix_seconds(epoch_seconds);
  ACU_ASSERT(unix_seconds == 2524608000);
}

ACU_TEST(test_atc_convert_from_unix_seconds)
{
  int64_t unix_seconds = 946684800; // 2000-01-01
  atc_time_t epoch_seconds = atc_convert_from_unix_seconds(unix_seconds);
  ACU_ASSERT(epoch_seconds == -1577923200);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_convert_to_unix_seconds);
  ACU_RUN_TEST(test_atc_convert_from_unix_seconds);
  ACU_SUMMARY();
}
