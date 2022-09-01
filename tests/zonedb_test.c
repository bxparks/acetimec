#include <acunit.h>
#include <acetimec.h>

ACU_TEST(test_zonedb_sizes)
{
  // These numbers are correct for TZDB 2022a
  ACU_ASSERT(sizeof(kAtcZoneRegistry) / sizeof(AtcZoneInfo*) == 356);
  ACU_ASSERT(sizeof(kAtcZoneAndLinkRegistry) / sizeof(AtcZoneInfo*)
      == 595);
  ACU_ASSERT(sizeof(kAtcLinkRegistry) / sizeof(AtcLinkEntry) == 239);
}

//---------------------------------------------------------------------------

ACU_VARS();

int main()
{
  ACU_RUN_TEST(test_zonedb_sizes);
  ACU_SUMMARY();
}
