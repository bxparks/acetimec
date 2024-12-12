#include <acunit.h>
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
}

ACU_TEST(test_atc_copy_replace_string_out_of_bounds) {
  const uint8_t dst_size = 3;
  char dst[dst_size];

  atc_copy_replace_string(dst, dst_size, "E%T%Z", '%', "suv");
  ACU_ASSERT(strcmp("Es", dst) == 0);
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_djb2)
{
  ACU_ASSERT((uint32_t) 5381 == atc_djb2(""));
  ACU_ASSERT((uint32_t) 177670 == atc_djb2("a"));
  ACU_ASSERT((uint32_t) 177671 == atc_djb2("b"));
  ACU_ASSERT((uint32_t) 5863208 == atc_djb2("ab"));
  ACU_ASSERT((uint32_t) 193485963 == atc_djb2("abc"));
  ACU_ASSERT((uint32_t) 2090069583 == atc_djb2("abcd"));
  ACU_ASSERT((uint32_t) 252819604 == atc_djb2("abcde"));
}

//---------------------------------------------------------------------------

ACU_TEST(test_atc_seconds_to_hms)
{
  uint16_t hh, mm, ss;

  atc_seconds_to_hms(0, &hh, &mm, &ss);
  ACU_ASSERT((uint16_t) 0 == hh);
  ACU_ASSERT((uint16_t) 0 == mm);
  ACU_ASSERT((uint16_t) 0 == ss);

  atc_seconds_to_hms(3600, &hh, &mm, &ss);
  ACU_ASSERT((uint16_t) 1 == hh);
  ACU_ASSERT((uint16_t) 0 == mm);
  ACU_ASSERT((uint16_t) 0 == ss);

  atc_seconds_to_hms(3720, &hh, &mm, &ss);
  ACU_ASSERT((uint16_t) 1 == hh);
  ACU_ASSERT((uint16_t) 2 == mm);
  ACU_ASSERT((uint16_t) 0 == ss);

  atc_seconds_to_hms(3723, &hh, &mm, &ss);
  ACU_ASSERT((uint16_t) 1 == hh);
  ACU_ASSERT((uint16_t) 2 == mm);
  ACU_ASSERT((uint16_t) 3 == ss);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_copy_replace_string_normal);
  ACU_RUN_TEST(test_atc_copy_replace_string_out_of_bounds);
  ACU_RUN_TEST(test_atc_djb2);
  ACU_RUN_TEST(test_atc_seconds_to_hms);
  ACU_SUMMARY();
}
