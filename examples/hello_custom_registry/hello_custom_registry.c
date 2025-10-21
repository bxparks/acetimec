/*
Sample program to demonstrate the use of a custom zone registry.

Usage:
$ make

$ ./hello_custom_registry.out
==== Los Angeles from PlainDateTime
Los Angeles: 2019-03-10T03:00:00-07:00[America/Los_Angeles]
Epoch seconds: -972396000
Unix seconds: 1552212000
==== Convert ZonedDateTime to different time zone
New York: 2019-03-10T06:00:00-04:00[America/New_York]
Epoch seconds: -972396000
Unix seconds: 1552212000
*/

#include <stdlib.h> // exit()
#include <stdio.h>
#include <acetimec.h>

// Initialize the custom registry with 5 zones, from the zonedb2025 database.
const AtcZoneInfo * const kCustomRegistry[]  = {
  &kAtcZonedb2025ZoneAmerica_Los_Angeles, // 0xb7f7e8f2, America/Los_Angeles
  &kAtcZonedb2025ZoneAmerica_Denver, // 0x97d10b2a, America/Denver
  &kAtcZonedb2025ZoneAmerica_Chicago, // 0x4b92b5d4, America/Chicago
  &kAtcZonedb2025ZoneAmerica_New_York, // 0x1e2a7654, America/New_York
  &kAtcZonedb2025ZoneEurope_London, // 0x5c6a84ae, Europe/London
};

const uint16_t kCustomRegistrySize = sizeof(kCustomRegistry) /
    sizeof(const AtcZoneInfo *);

// Registrar can be configured with the custom registry.
AtcZoneRegistrar registrar;

// Two zones are used concurrently in this program.
AtcZoneProcessor processor1;
AtcZoneProcessor processor2;

// Initialize the registrar and the time zone processors.
void setup()
{
  atc_registrar_init(&registrar, kCustomRegistry, kCustomRegistrySize);
  atc_processor_init(&processor1);
  atc_processor_init(&processor2);
}

void print_dates()
{
  printf("==== Los Angeles from PlainDateTime\n");

  // Retrieve America/Los_Angeles.
  const AtcZoneInfo *info = atc_registrar_find_by_name(
      &registrar, "America/Los_Angeles");
  AtcTimeZone tz1 = {info, &processor1};
  AtcZonedDateTime zdt1;

  // Convert 2019-03-10 03:00:00 to Los Angeles time zone.
  AtcPlainDateTime pdt1 = {2019, 3, 10, 3, 0, 0};
  atc_zoned_date_time_from_plain_date_time(
      &zdt1, &pdt1, &tz1, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdt1)) {
    printf("ERROR: Unable to create ZonedDateTime from PlainDateTime\n");
    exit(1);
  }

  // Print the date for Los Angeles.
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, sizeof(buf));
  atc_zoned_date_time_print(&sb, &zdt1);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Print the epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt1);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  int64_t unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdt1);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== Convert ZonedDateTime to different time zone\n");

  // convert America/Los_Angeles to America/New_York
  info = atc_registrar_find_by_name(&registrar, "America/New_York");
  AtcTimeZone tz2 = {info, &processor2};
  AtcZonedDateTime zdt2;

  atc_zoned_date_time_convert(&zdt1, &tz2, &zdt2);
  if (atc_zoned_date_time_is_error(&zdt2)) {
    printf("ERROR: Unable to convert ZonedDateTime to New York time zone\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdt2);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt2);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdt1);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  setup();
  print_dates();
}
