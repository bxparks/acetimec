#include "acunit.h"
#include <string.h>
#include <acetimec.h>

ACU_TEST(test_atc_copy_replace_string_normal)
{
  const uint8_t dst_size = 20;
  char dst[dst_size];

  atc_copy_replace_string(dst, dst_size, "NOREPLACE", '%', "suv");
  ACU_ASSERT(strcmp("NOREPLACE", dst) == 0);

  atc_copy_replace_string(dst, dst_size, "E%T%Z", '%', "suv");
  ACU_ASSERT(strcmp("EsuvTsuvZ", dst) == 0);

  atc_copy_replace_string(dst, dst_size, "E%T", '%', "");
  ACU_ASSERT(strcmp("ET", dst) == 0);

  ACU_PASS();
}

ACU_TEST(test_atc_copy_replace_string_out_of_bounds) {
  const uint8_t dst_size = 3;
  char dst[dst_size];

  atc_copy_replace_string(dst, dst_size, "E%T%Z", '%', "suv");
  ACU_ASSERT(strcmp("Es", dst) == 0);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_copy_replace_string_normal);
  ACU_RUN_TEST(test_atc_copy_replace_string_out_of_bounds);
  ACU_SUMMARY();
}
