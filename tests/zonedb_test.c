#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_zonedb_sizes)
{
  // These numbers are correct for TZDB 2022g
  ACU_ASSERT(sizeof(kAtcAllZoneRegistry) / sizeof(AtcZoneInfo*) == 351);
  ACU_ASSERT(sizeof(kAtcAllZoneAndLinkRegistry) / sizeof(AtcZoneInfo*)
      == 596);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zonedb_sizes);
  ACU_SUMMARY();
}
