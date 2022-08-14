#include "acunit.h"
#include <acetimec.h>

ACU_TEST(test_atc_registrar_find_by_name)
{
  const struct AtcZoneInfo *info = atc_registrar_find_by_name(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      "America/Los_Angeles");
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  info = atc_registrar_find_by_name(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      "should not exist");
  ACU_ASSERT(info == NULL);

  ACU_PASS();
}

ACU_TEST(test_atc_registrar_find_by_id)
{
  const struct AtcZoneInfo *info = atc_registrar_find_by_id(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      0xb7f7e8f2);
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  const uint32_t should_not_exist = 0x0;
  info = atc_registrar_find_by_id(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      should_not_exist);
  ACU_ASSERT(info == NULL);

  ACU_PASS();
}

//---------------------------------------------------------------------------

ACU_PARAMS();

int main()
{
  ACU_RUN_TEST(test_atc_registrar_find_by_name);
  ACU_RUN_TEST(test_atc_registrar_find_by_id);
  ACU_SUMMARY();
}
