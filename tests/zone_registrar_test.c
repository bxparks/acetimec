#include <acunit.h>
#include <acetimec.h>

static const AtcZoneInfo * const kUnsortedRegistry[] = {
  &kAtcTestingZoneAmerica_Los_Angeles,
  &kAtcTestingZoneAmerica_Denver,
  &kAtcTestingZoneAmerica_Chicago,
  &kAtcTestingZoneAmerica_New_York,
};

#define UNSORTED_SIZE \
    (sizeof(kUnsortedRegistry) / sizeof(const AtcZoneInfo *))

ACU_TEST(test_atc_registrar_is_registry_sorted)
{
  bool is_sorted = atc_registrar_is_registry_sorted(
      kAtcTestingZoneAndLinkRegistry,
      kAtcTestingZoneAndLinkRegistrySize);
  ACU_ASSERT(is_sorted == true);

  is_sorted = atc_registrar_is_registry_sorted(
      kUnsortedRegistry,
      UNSORTED_SIZE);
  ACU_ASSERT(is_sorted == false);
}

ACU_TEST(test_atc_registrar_find_by_id_sorted)
{
  AtcZoneRegistrar registrar;
  atc_registrar_init(
      &registrar,
      kAtcTestingZoneAndLinkRegistry,
      kAtcTestingZoneAndLinkRegistrySize);

  const AtcZoneInfo *info = atc_registrar_find_by_id(&registrar, 0xb7f7e8f2);
  ACU_ASSERT(info == &kAtcTestingZoneAmerica_Los_Angeles);

  const uint32_t should_not_exist = 0x0;
  info = atc_registrar_find_by_id(&registrar, should_not_exist);
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_name_sorted)
{
  AtcZoneRegistrar registrar;
  atc_registrar_init(
      &registrar,
      kAtcTestingZoneAndLinkRegistry,
      kAtcTestingZoneAndLinkRegistrySize);

  const AtcZoneInfo *info = atc_registrar_find_by_name(
      &registrar, "America/Los_Angeles");
  ACU_ASSERT(info == &kAtcTestingZoneAmerica_Los_Angeles);

  info = atc_registrar_find_by_name(
      &registrar, "should not exist");
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_id_unsorted)
{
  AtcZoneRegistrar registrar;
  atc_registrar_init( &registrar, kUnsortedRegistry, UNSORTED_SIZE);

  const AtcZoneInfo *info = atc_registrar_find_by_id(&registrar, 0xb7f7e8f2);
  ACU_ASSERT(info == &kAtcTestingZoneAmerica_Los_Angeles);

  const uint32_t should_not_exist = 0x0;
  info = atc_registrar_find_by_id(&registrar, should_not_exist);
  ACU_ASSERT(info == NULL);
}

ACU_TEST(test_atc_registrar_find_by_name_unsorted)
{
  AtcZoneRegistrar registrar;
  atc_registrar_init(&registrar, kUnsortedRegistry, UNSORTED_SIZE);

  const AtcZoneInfo *info = atc_registrar_find_by_name(
      &registrar, "America/Los_Angeles");
  ACU_ASSERT(info == &kAtcTestingZoneAmerica_Los_Angeles);

  info = atc_registrar_find_by_name(
      &registrar, "should not exist");
  ACU_ASSERT(info == NULL);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_atc_registrar_is_registry_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_id_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_name_sorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_id_unsorted);
  ACU_RUN_TEST(test_atc_registrar_find_by_name_unsorted);
  ACU_SUMMARY();
}
