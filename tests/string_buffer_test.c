#include <acunit.h>
#include <string.h>
#include <acetimec.h>

char buf[80];
AtcStringBuffer sb;

ACU_TEST(test_atc_print_char)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_char(&sb, 'a');
  atc_buf_close(&sb);

  ACU_ASSERT(sb.size == 1);
  ACU_ASSERT(sb.p[0] == 'a');
  ACU_ASSERT(sb.p[1] == '\0');
}

ACU_TEST(test_atc_buf_reset)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_char(&sb, 'a');
  atc_buf_close(&sb);

  ACU_ASSERT(sb.size == 1);
  ACU_ASSERT(sb.p[0] == 'a');
  ACU_ASSERT(sb.p[1] == '\0');

  atc_buf_reset(&sb);
  ACU_ASSERT(sb.size == 0);
}

ACU_TEST(test_atc_print_string)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_string(&sb, "hello");
  atc_buf_close(&sb);

  ACU_ASSERT(sb.size == 5);
  ACU_ASSERT(strcmp(sb.p, "hello") == 0);
}

ACU_TEST(test_atc_print_uint16)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_uint16(&sb, 123);
  atc_buf_close(&sb);

  ACU_ASSERT(sb.size == 3);
  ACU_ASSERT(strcmp(sb.p, "123") == 0);
}

ACU_TEST(test_atc_print_uint16_pad2)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad2(&sb, 3);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 2);
  ACU_ASSERT(strcmp(sb.p, "03") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad2(&sb, 23);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 2);
  ACU_ASSERT(strcmp(sb.p, "23") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad2(&sb, 123);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 3);
  ACU_ASSERT(strcmp(sb.p, "123") == 0);
}

ACU_TEST(test_atc_print_uint16_pad4)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad4(&sb, 1);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 4);
  ACU_ASSERT(strcmp(sb.p, "0001") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad4(&sb, 12);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 4);
  ACU_ASSERT(strcmp(sb.p, "0012") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad4(&sb, 123);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 4);
  ACU_ASSERT(strcmp(sb.p, "0123") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad4(&sb, 1234);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 4);
  ACU_ASSERT(strcmp(sb.p, "1234") == 0);

  atc_buf_init(&sb, buf, 80);
  atc_print_uint16_pad4(&sb, 12345);
  atc_buf_close(&sb);
  ACU_ASSERT(sb.size == 5);
  ACU_ASSERT(strcmp(sb.p, "12345") == 0);
}

ACU_TEST(test_atc_print_int16)
{
  atc_buf_init(&sb, buf, 80);
  atc_print_int16(&sb, -123);
  atc_buf_close(&sb);

  ACU_ASSERT(sb.size == 4);
  ACU_ASSERT(strcmp(sb.p, "-123") == 0);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_atc_print_char);
  ACU_RUN_TEST(test_atc_buf_reset);
  ACU_RUN_TEST(test_atc_print_string);
  ACU_RUN_TEST(test_atc_print_uint16);
  ACU_RUN_TEST(test_atc_print_uint16_pad2);
  ACU_RUN_TEST(test_atc_print_uint16_pad4);
  ACU_RUN_TEST(test_atc_print_int16);
  ACU_SUMMARY();
}
