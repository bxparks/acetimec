#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_plain_time_is_valid)
{
  // valid times
  ACU_ASSERT(atc_plain_time_is_valid(0, 0, 0));
  ACU_ASSERT(atc_plain_time_is_valid(0, 0, 59));
  ACU_ASSERT(atc_plain_time_is_valid(0, 59, 0));
  ACU_ASSERT(atc_plain_time_is_valid(23, 0, 0));
  ACU_ASSERT(atc_plain_time_is_valid(23, 59, 59));

  // invalid times
  ACU_ASSERT(! atc_plain_time_is_valid(0, 0, 60));
  ACU_ASSERT(! atc_plain_time_is_valid(0, 60, 0));
  ACU_ASSERT(! atc_plain_time_is_valid(24, 0, 0));
}

ACU_TEST(test_plain_time_to_seconds)
{
  ACU_ASSERT(0 == atc_plain_time_to_seconds(0, 0, 0));
  ACU_ASSERT(1 == atc_plain_time_to_seconds(0, 0, 1));
  ACU_ASSERT(60 == atc_plain_time_to_seconds(0, 1, 0));
  ACU_ASSERT(3600 == atc_plain_time_to_seconds(1, 0, 0));
  ACU_ASSERT(3661 == atc_plain_time_to_seconds(1, 1, 1));
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_plain_time_is_valid);
  ACU_RUN_TEST(test_plain_time_to_seconds);
  ACU_SUMMARY();
}
