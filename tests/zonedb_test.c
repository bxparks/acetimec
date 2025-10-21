#include <acunit.h>
#include <acetimec.h>

// These tests often break and need to be updated when the TZDB is upgraded to a
// new version. In that sense, they are somewhat annoying. On the other hand,
// they run the simplest sanity check, reading the total number of zones. They
// are useful when we do a major refactoring and the unit tests fall apart. It's
// useful to have this as a baseline so that we can rebuild the rest of the
// testing infrastructure.
// 

ACU_TEST(test_zonedb2000_sizes)
{
  // These numbers are correct for TZDB 2025b
  ACU_ASSERT(sizeof(kAtcZonedb2000ZoneRegistry) / sizeof(AtcZoneInfo*) == 340);
  ACU_ASSERT(sizeof(kAtcZonedb2000ZoneAndLinkRegistry) / sizeof(AtcZoneInfo*)
      == 597);
}

ACU_TEST(test_zonedb2025_sizes)
{
  // These numbers are correct for TZDB 2025b
  ACU_ASSERT(sizeof(kAtcZonedb2025ZoneRegistry) / sizeof(AtcZoneInfo*) == 340);
  ACU_ASSERT(sizeof(kAtcZonedb2025ZoneAndLinkRegistry) / sizeof(AtcZoneInfo*)
      == 597);
}

#if ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_HIGH
ACU_TEST(test_zonedball_sizes)
{
  // These numbers are correct for TZDB 2025b
  ACU_ASSERT(sizeof(kAtcZonedballZoneRegistry) / sizeof(AtcZoneInfo*) == 340);
  ACU_ASSERT(sizeof(kAtcZonedballZoneAndLinkRegistry) / sizeof(AtcZoneInfo*)
      == 597);
}
#endif

//---------------------------------------------------------------------------

ACU_CONTEXT();

int main()
{
  ACU_RUN_TEST(test_zonedb2000_sizes);
  ACU_RUN_TEST(test_zonedb2025_sizes);
#if ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_HIGH
  ACU_RUN_TEST(test_zonedball_sizes);
#endif
  ACU_SUMMARY();
}
