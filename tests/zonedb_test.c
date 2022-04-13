#include "acunit.h"
#include <acetimec.h>

acu_test(test_zonedb_sizes)
{
  // These numbers are correct for TZDB 2022a
  acu_assert(sizeof(kAtcZoneRegistry) / sizeof(struct AtcZoneInfo*) == 377);
  acu_assert(sizeof(kAtcZoneAndLinkRegistry) / sizeof(struct AtcZoneInfo*)
      == 594);
  acu_assert(sizeof(kAtcLinkRegistry) / sizeof(struct AtcLinkEntry)
      == 217);
  acu_pass();
}

//---------------------------------------------------------------------------

int acu_tests_run = 0;
int acu_tests_failed = 0;

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;

  acu_run_test(test_zonedb_sizes);
  acu_summary();
}
