/*
A more complex sample program that builds on top of examples/hello_acetimec.

Usage:
$ make
$ ./hello_acetimec_more.out
==== Epoch year
Current epoch year: 2050
==== ZonedDateTime from epoch seconds
Epoch seconds: 3432423
Los Angeles: 2050-02-09T09:27:03-08:00[America/Los_Angeles]
Resolved: 0
Epoch seconds: 3432423
Unix seconds: 2528040423
==== ZonedDateTime from PlainDateTime in gap
PlainDateTime: 2022-03-13T02:30:00
Los Angeles: 2022-03-13T03:30:00-07:00[America/Los_Angeles]
Resolved: 4
Epoch seconds: -877440600
Unix seconds: 1647167400
==== ZonedDateTime from PlainDateTime in overlap
PlainDateTime: 2022-11-06T01:30:00
Los Angeles: 2022-11-06T01:30:00-07:00[America/Los_Angeles]
Resolved: 1
Epoch seconds: -856884600
Unix seconds: 1667723400
==== Convert ZonedDateTime to different time zone
New York: 2022-11-06T03:30:00-05:00[America/New_York]
Resolved: 0
Epoch seconds: -856884600
Unix seconds: 1667723400
==== Modify ZonedDateTime without normalization
New York: 2022-11-06T00:30:00-05:00[America/New_York]
Resolved: 0
Epoch seconds: -856895400
Unix seconds: 1667712600
==== Normalize ZonedDateTime using DisambiguateCompatible
New York: 2022-11-06T00:30:00-04:00[America/New_York]
Resolved: 0
Epoch seconds: -856899000
Unix seconds: 1667709000
*/

#include <stdlib.h> // exit()
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
  printf("==== Epoch year\n");
  int16_t current_epoch_year = atc_get_current_epoch_year();
  printf("Current epoch year: %d\n", (int) current_epoch_year);

  printf("==== ZonedDateTime from epoch seconds\n");

  atc_time_t seconds = 3432423;
  printf("Epoch seconds: %ld\n", (long) seconds);

  // Convert epoch seconds to date/time components for given time zone.
  AtcTimeZone tzla = {&kAtcZonedb2000ZoneAmerica_Los_Angeles, &processor_la};
  AtcZonedDateTime zdtla;
  atc_zoned_date_time_from_epoch_seconds(&zdtla, seconds, &tzla);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from epoch seconds\n");
    exit(1);
  }

  // Allocate string buffer for human readable strings.
  struct AtcStringBuffer sb;
  char buf[80];
  atc_buf_init(&sb, buf, sizeof(buf));

  // Print the date for Los Angeles.
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);
  printf("Resolved: %d\n", zdtla.resolved);

  // Print the epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  if (seconds != epoch_seconds) {
    printf("ERROR: Converted seconds (%ld) != original seconds (%ld)\n",
        (long) epoch_seconds, (long) seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  int64_t unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== ZonedDateTime from PlainDateTime in gap\n");

  // Start with a PlainDateTime in an gap (02:00 -> 03:00).
  AtcPlainDateTime pdt = {2022, 3, 13, 2, 30, 0};
  atc_buf_reset(&sb);
  atc_plain_date_time_print(&sb, &pdt);
  atc_buf_close(&sb);
  printf("PlainDateTime: %s\n", sb.p);

  // Convert components to an AtcZonedDateTime. 2022-11-06 01:30 occurred twice.
  // It is probably most common to want the earlier one, which can be done
  // using either kAtcDisambiguateCompatible or kAtcDisambiguateEarlier.
  atc_zoned_date_time_from_plain_date_time(
      &zdtla, &pdt, &tzla, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from PlainDateTime\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);
  printf("Resolved: %d\n", zdtla.resolved);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== ZonedDateTime from PlainDateTime in overlap\n");

  // Start with a PlainDateTime in an overlap (02:00 -> 01:00).
  pdt = (AtcPlainDateTime) {2022, 11, 6, 1, 30, 0};
  atc_buf_reset(&sb);
  atc_plain_date_time_print(&sb, &pdt);
  atc_buf_close(&sb);
  printf("PlainDateTime: %s\n", sb.p);

  // Convert components to an AtcZonedDateTime. 2022-11-06 01:30 occurred twice.
  // It is probably most common to want the earlier one, which can be done
  // using either kAtcDisambiguateCompatible or kAtcDisambiguateEarlier.
  atc_zoned_date_time_from_plain_date_time(
      &zdtla, &pdt, &tzla, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from PlainDateTime\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);
  printf("Resolved: %d\n", zdtla.resolved);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== Convert ZonedDateTime to different time zone\n");

  // convert America/Los_Angeles to America/New_York
  AtcTimeZone tzny = {&kAtcZonedb2000ZoneAmerica_New_York, &processor_ny};
  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, &tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtny)) {
    printf("ERROR: Unable to convert ZonedDateTime to New York time zone\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  printf("Resolved: %d\n", zdtny.resolved);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtny);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtny);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== Modify ZonedDateTime without normalization\n");

  // Manually modify the date time in New York to 2022-11-06T00:30:00. The
  // object's UTC offset remains at -05:00, but should be -04:00 because
  // 2022-11-06T00:30:00 is still under DST.
  zdtny.hour = 0;
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtny);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds for modified New York date time\n");
    exit(1);
  }

  // Print the incorrect epoch seconds.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  printf("Resolved: %d\n", zdtny.resolved);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtny);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtny);
  if (unix_seconds == kAtcInvalidUnixSeconds) {
    printf("ERROR: Invalid unix seconds %lld; should not happen\n",
        (long long) unix_seconds);
    exit(1);
  }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== Normalize ZonedDateTime using DisambiguateCompatible\n");

  // Normalize to the later time in the overlap
  atc_zoned_date_time_normalize(&zdtny, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtny)) {
    printf("ERROR: Unable to normalize ZonedDateTime\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  printf("Resolved: %d\n", zdtny.resolved);

  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtny);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Invalid epoch seconds %ld; should not happen\n",
        (long) epoch_seconds);
    exit(1);
  }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtny);
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
