/*
Sample program used the README.md file.

Usage:
$ make
$ ./hello_acetimec.out
======== ZonedDateTime from epoch seconds
Epoch Seconds: 3432423
Resolved: 0
Los Angeles: 2050-02-09T09:27:03-08:00[America/Los_Angeles]
Converted Seconds: 3432423
======== ZonedDateTime from LocalDateTime using DisambiguateCompatible
LocalDateTime: 2022-11-06T01:30:00
Resolved: 1
Los Angeles: 2022-11-06T01:30:00-07:00[America/Los_Angeles]
Epoch Seconds: -856884600
======== Convert ZonedDateTime to different time zone
Resolved: 0
New York: 2022-11-06T03:30:00-05:00[America/New_York]
Epoch Seconds: -856884600
======== Modify ZonedDateTime without normalization
Resolved: 0
New York: 2022-11-06T00:30:00-05:00[America/New_York]
Epoch Seconds: -856884600
======== Normalize ZonedDateTime using DisambiguateCompatible
Resolved: 0
New York: 2022-11-06T00:30:00-04:00[America/New_York]
Epoch Seconds: -856884600
*/

#include <stdlib.h> // exit()
#include <stdio.h>
#include <acetimec.h>

AtcZoneProcessor processor_la; // Los Angeles
AtcZoneProcessor processor_ny; // New York

// initialize the time zone processor workspace
void setup()
{
  atc_processor_init(&processor_la);
  atc_processor_init(&processor_ny);
}

void print_dates()
{
  printf("======== ZonedDateTime from epoch seconds\n");

  atc_time_t seconds = 3432423;
  printf("Epoch Seconds: %ld\n", (long) seconds);

  // Convert epoch seconds to date/time components for given time zone.
  AtcTimeZone tzla = {&kAtcZoneAmerica_Los_Angeles, &processor_la};
  AtcZonedDateTime zdtla;
  atc_zoned_date_time_from_epoch_seconds(&zdtla, seconds, &tzla);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from epoch seconds\n");
    exit(1);
  }

  // Print the date for Los Angeles.
  printf("Resolved: %d\n", zdtla.resolved);
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Convert zoned_date_time to back to epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) {
    printf("ERROR: Unable to convert ZonedDateTime to epoch seconds\n");
    exit(1);
  }
  if (seconds != epoch_seconds) {
    printf("ERROR: Converted seconds (%ld) != original seconds (%ld)\n",
        (long) epoch_seconds, (long) seconds);
    exit(1);
  }
  printf("Converted Seconds: %ld\n", (long) epoch_seconds);

  printf("======== ZonedDateTime from LocalDateTime using DisambiguateCompatible\n");

  // Start with a LocalDateTime in an overlap.
  AtcLocalDateTime ldt = {2022, 11, 6, 1, 30, 0};
  atc_buf_reset(&sb);
  atc_local_date_time_print(&sb, &ldt);
  atc_buf_close(&sb);
  printf("LocalDateTime: %s\n", sb.p);

  // Convert components to an AtcZonedDateTime. 2022-11-06 01:30 occurred twice.
  // It is probably most common to want the earlier one, which can be done
  // using either kAtcDisambiguateCompatible or kAtcDisambiguateEarlier.
  atc_zoned_date_time_from_local_date_time(
      &zdtla, &ldt, &tzla, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from LocalDateTime\n");
    exit(1);
  }

  // Print the date time.
  printf("Resolved: %d\n", zdtla.resolved);
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);

  printf("======== Convert ZonedDateTime to different time zone\n");

  // convert America/Los_Angeles to America/New_York
  AtcTimeZone tzny = {&kAtcZoneAmerica_New_York, &processor_ny};
  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, &tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtny)) {
    printf("ERROR: Unable to convert ZonedDateTime to New York time zone\n");
    exit(1);
  }

  // Print the date time.
  printf("Resolved: %d\n", zdtny.resolved);
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);

  printf("======== Modify ZonedDateTime without normalization\n");

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
  printf("Resolved: %d\n", zdtny.resolved);
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);

  printf("======== Normalize ZonedDateTime using DisambiguateCompatible\n");

  // Normalize to the later time in the overlap
  atc_zoned_date_time_normalize(&zdtny, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtny)) {
    printf("ERROR: Unable to normalize ZonedDateTime\n");
    exit(1);
  }

  // Print the date time.
  printf("Resolved: %d\n", zdtny.resolved);
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  setup();
  print_dates();
}
