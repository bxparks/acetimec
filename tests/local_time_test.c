#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_local_time_to_seconds)
{
  ACU_ASSERT(0 == atc_local_time_to_seconds(0, 0, 0));
  ACU_ASSERT(1 == atc_local_time_to_seconds(0, 0, 1));
  ACU_ASSERT(60 == atc_local_time_to_seconds(0, 1, 0));
  ACU_ASSERT(3600 == atc_local_time_to_seconds(1, 0, 0));
  ACU_ASSERT(3661 == atc_local_time_to_seconds(1, 1, 1));
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_local_time_to_seconds);
  ACU_SUMMARY();
}
