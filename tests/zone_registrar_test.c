#include <acunit.h>
#include <acetimec.h>

static const AtcZoneInfo * const kUnsortedRegistry[] = {
  &kAtcZoneAmerica_Los_Angeles,
  &kAtcZoneAmerica_Denver,
  &kAtcZoneAmerica_Chicago,
  &kAtcZoneAmerica_New_York,
};

#define UNSORTED_SIZE \
    (sizeof(kUnsortedRegistry) / sizeof(const AtcZoneInfo *))

ACU_TEST(test_atc_registrar_is_registry_sorted)
{
  bool is_sorted = atc_registrar_is_registry_sorted(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize);
  ACU_ASSERT(is_sorted == true);

  is_sorted = atc_registrar_is_registry_sorted(
      kUnsortedRegistry,
      UNSORTED_SIZE);
  ACU_ASSERT(is_sorted == false);
}

ACU_TEST(test_atc_registrar_find_by_id_sorted)
{
  const AtcZoneInfo *info = atc_registrar_find_by_id(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      0xb7f7e8f2,
      true);
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  const uint32_t should_not_exist = 0x0;
  info = atc_registrar_find_by_id(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      should_not_exist,
      true);
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_name_sorted)
{
  const AtcZoneInfo *info = atc_registrar_find_by_name(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      "America/Los_Angeles",
      true);
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  info = atc_registrar_find_by_name(
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize,
      "should not exist",
      true);
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_id_unsorted)
{
  const AtcZoneInfo *info = atc_registrar_find_by_id(
      kUnsortedRegistry,
      UNSORTED_SIZE,
      0xb7f7e8f2,
      false);
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  const uint32_t should_not_exist = 0x0;
  info = atc_registrar_find_by_id(
      kUnsortedRegistry,
      UNSORTED_SIZE,
      should_not_exist,
      false);
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_name_unsorted)
{
  const AtcZoneInfo *info = atc_registrar_find_by_name(
      kUnsortedRegistry,
      UNSORTED_SIZE,
      "America/Los_Angeles",
      false);
  ACU_ASSERT(info == &kAtcZoneAmerica_Los_Angeles);

  info = atc_registrar_find_by_name(
      kUnsortedRegistry,
      UNSORTED_SIZE,
      "should not exist",
      false);
  ACU_ASSERT(info == NULL);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_atc_registrar_is_registry_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_id_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_name_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_id_unsorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_name_unsorted);
  ACU_SUMMARY();
}
