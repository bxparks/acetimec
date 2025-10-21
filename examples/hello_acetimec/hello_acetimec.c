/*
Sample program used in the README.md file that demonstrates the basic features
of the acetimec library. WARNING: This performs no error checking to reduce
clutter for demo purposes.

Usage:
$ make
$ ./hello_acetimec.out
==== ZonedDateTime from epoch seconds
Epoch seconds: 3432423
Los Angeles: 2050-02-09T09:27:03-08:00[America/Los_Angeles]
Epoch seconds: 3432423
Unix seconds: 2528040423
==== ZonedDateTime from PlainDateTime with DisambiguateCompatible
PlainDateTime: 2022-11-06T01:30:00
Los Angeles: 2022-11-06T01:30:00-07:00[America/Los_Angeles]
Epoch seconds: -856884600
Unix seconds: 1667723400
==== Convert ZonedDateTime to different time zone
New York: 2022-11-06T03:30:00-05:00[America/New_York]
Epoch seconds: -856884600
Unix seconds: 1667723400
*/

#include <stdio.h>
#include <acetimec.h>

AtcZoneProcessor processor_la; // Los Angeles
AtcZoneProcessor processor_ny; // New York

// Initialize the time zone processor workspace.
void setup()
{
  atc_processor_init(&processor_la);
  atc_processor_init(&processor_ny);
}

void print_dates()
{
  printf("==== ZonedDateTime from epoch seconds\n");

  atc_time_t seconds = 3432423;
  printf("Epoch seconds: %ld\n", (long) seconds);

  // Convert epoch seconds to date/time components for given time zone.
  AtcTimeZone tzla = {&kAtcZonedb2000ZoneAmerica_Los_Angeles, &processor_la};
  AtcZonedDateTime zdtla;
  atc_zoned_date_time_from_epoch_seconds(&zdtla, seconds, &tzla);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Allocate string buffer for human readable strings.
  struct AtcStringBuffer sb;
  char buf[80];
  atc_buf_init(&sb, buf, sizeof(buf));

  // Print the date for Los Angeles.
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Print the epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  if (seconds != epoch_seconds) { /*error*/ }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  int64_t unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== ZonedDateTime from PlainDateTime with DisambiguateCompatible\n");

  // Start with a PlainDateTime in an overlap.
  AtcPlainDateTime pdt = {2022, 11, 6, 1, 30, 0};
  atc_buf_reset(&sb);
  atc_plain_date_time_print(&sb, &pdt);
  atc_buf_close(&sb);
  printf("PlainDateTime: %s\n", sb.p);

  // Convert components to an AtcZonedDateTime. 2022-11-06 01:30 occurred twice.
  // It is probably most common to want the earlier one, which can be done
  // using either kAtcDisambiguateCompatible or kAtcDisambiguateEarlier.
  atc_zoned_date_time_from_plain_date_time(
      &zdtla, &pdt, &tzla, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== Convert ZonedDateTime to different time zone\n");

  // convert America/Los_Angeles to America/New_York
  AtcTimeZone tzny = {&kAtcZonedb2000ZoneAmerica_New_York, &processor_ny};
  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, &tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  setup();
  print_dates();
}
