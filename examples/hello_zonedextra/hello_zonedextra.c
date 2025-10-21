/*
Sample program to demonstrate using AtcZonedExtra to extract additional
information about a particular date-time instant.

Usage:
$ make
$ ./hello_zonedextra.out
==== Before gap at 2022-03-13 00:30:00
ZonedDateTime: 2022-03-13T00:30:00-08:00[America/Los_Angeles]
ZonedDateTime.resolved: 0
ZonedExtra.abbrev: PST
ZonedExtra.fold_type: 1
==== Inside gap at 2022-03-13 02:30:00
ZonedDateTime: 2022-03-13T03:30:00-07:00[America/Los_Angeles]
ZonedDateTime.resolved: 4
ZonedExtra.abbrev: PDT
ZonedExtra.fold_type: 3
==== In Daylight time at 2022-03-13 03:30:00
ZonedDateTime: 2022-03-13T03:30:00-07:00[America/Los_Angeles]
ZonedDateTime.resolved: 0
ZonedExtra.abbrev: PDT
ZonedExtra.fold_type: 1
==== In overlap at 2022-11-06 01:30:00
ZonedDateTime: 2022-11-06T01:30:00-07:00[America/Los_Angeles]
ZonedDateTime.resolved: 1
ZonedExtra.abbrev: PDT
ZonedExtra.fold_type: 2
==== In Standard time at 2022-11-06 02:30:00
ZonedDateTime: 2022-11-06T02:30:00-08:00[America/Los_Angeles]
ZonedDateTime.resolved: 0
ZonedExtra.abbrev: PST
ZonedExtra.fold_type: 1
*/

#include <stdlib.h> // exit()
#include <stdio.h>
#include <acetimec.h>

// Only a single zone used in this program.
AtcZoneProcessor processor;

// Initialize the registrar and the time zone processors.
void setup()
{
  atc_processor_init(&processor);
}

void print_info(const char *label, AtcPlainDateTime *pdt, AtcTimeZone *tz)
{
  printf("==== %s\n", label);

  AtcZonedDateTime zdt;
  atc_zoned_date_time_from_plain_date_time(
      &zdt, pdt, tz, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdt)) {
    printf("ERROR: Unable to create ZonedDateTime from PlainDateTime\n");
    exit(1);
  }

  // Print the date for Los Angeles.
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, sizeof(buf));
  atc_zoned_date_time_print(&sb, &zdt);
  atc_buf_close(&sb);
  printf("ZonedDateTime: %s\n", sb.p);
  printf("ZonedDateTime.resolved: %d\n", zdt.resolved);

  // Get ZonedExtra at that time.
  AtcZonedExtra extra;
  atc_zoned_extra_from_plain_date_time(
      &extra, pdt, tz, kAtcDisambiguateCompatible);
  if (atc_zoned_extra_is_error(&extra)) {
    printf("ERROR: Unable to create ZonedExtra from PlainDateTime\n");
    exit(1);
  }

  // Print the abbreviation and fold type (exact, overlap, gap).
  printf("ZonedExtra.abbrev: %s\n", extra.abbrev);
  printf("ZonedExtra.fold_type: %d\n", extra.fold_type);
}

void print_dates()
{
  AtcTimeZone tz = {&kAtcZonedb2000ZoneAmerica_Los_Angeles, &processor};

  AtcPlainDateTime pdt = {2022, 3, 13, 0, 30, 0};
  print_info("Before gap at 2022-03-13 00:30:00", &pdt, &tz);

  pdt = (AtcPlainDateTime) {2022, 3, 13, 2, 30, 0};
  print_info("Inside gap at 2022-03-13 02:30:00", &pdt, &tz);

  pdt = (AtcPlainDateTime) {2022, 3, 13, 3, 30, 0};
  print_info("In Daylight time at 2022-03-13 03:30:00", &pdt, &tz);

  pdt = (AtcPlainDateTime) {2022, 11, 6, 1, 30, 0};
  print_info("In overlap at 2022-11-06 01:30:00", &pdt, &tz);

  pdt = (AtcPlainDateTime) {2022, 11, 6, 2, 30, 0};
  print_info("In Standard time at 2022-11-06 02:30:00", &pdt, &tz);
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  setup();
  print_dates();
}
