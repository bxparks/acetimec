#include <acunit.h>
#include <acetimec.h>

// These tests often break and need to be updated when the TZDB is upgraded to a
// new version. In that sense, they are somewhat annoying. On the other hand,
// they run the simplest sanity check, reading the total number of zones. They
// are useful when we do a major refactoring and the unit tests fall apart. It's
// useful to have this as a baseline so that we can rebuild the rest of the
// testing infrastructure.
// 

ACU_TEST(test_zonedb_sizes)
{
  // These numbers are correct for TZDB 2024b
  ACU_ASSERT(sizeof(kAtcZoneRegistry) / sizeof(AtcZoneInfo*) == 339);
  ACU_ASSERT(sizeof(kAtcZoneAndLinkRegistry) / sizeof(AtcZoneInfo*) == 596);
}

ACU_TEST(test_zonedball_sizes)
{
  // These numbers are correct for TZDB 2024b
  ACU_ASSERT(sizeof(kAtcAllZoneRegistry) / sizeof(AtcZoneInfo*) == 339);
  ACU_ASSERT(sizeof(kAtcAllZoneAndLinkRegistry) / sizeof(AtcZoneInfo*) == 596);
}

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zonedb_sizes);
  ACU_RUN_TEST(test_zonedball_sizes);
  ACU_SUMMARY();
}
