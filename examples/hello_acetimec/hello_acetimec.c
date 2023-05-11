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

  printf("======== ZonedDateTime from LocalDateTime\n");

  // Start with a LocalDateTime in an overlap, fold=1 for the second one.
  AtcLocalDateTime ldt = {2022, 11, 6, 1, 30, 0, 1 /*fold*/};
  atc_buf_reset(&sb);
  atc_local_date_time_print(&sb, &ldt);
  atc_buf_close(&sb);
  printf("LocalDateTime: %s\n", sb.p);
  printf("fold: 1\n");

  // Convert components to zoned_date_time. 2022-11-06 01:30 occurred twice. Set
  // fold=1 to select the second occurrence.
  atc_zoned_date_time_from_local_date_time(&zdtla, &ldt, &tzla);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to create ZonedDateTime from LocalDateTime\n");
    exit(1);
  }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);

  printf("======== ZonedDateTime to different time zone\n");

  // convert America/Los_Angles to America/New_York
  AtcTimeZone tzny = {&kAtcZoneAmerica_New_York, &processor_ny};
  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, &tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtla)) {
    printf("ERROR: Unable to convert ZonedDateTime to New York time zone\n");
    exit(1);
  }

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
