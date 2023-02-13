/*
 * A demo program to determine the date-time in multiple time zones using the C
 * language and the standard C library.
 */
#include <stdio.h>
#include <stdlib.h> // setenv()
#include <string.h> //strncmp()
#include <time.h> // time(), localtime_r()
#include <stdlib.h> // setenv()
#include <stdbool.h> // bool
#include <unistd.h> // sleep()

/**
 * Set the timezone as currently specified by the "TZ" environment variable.
 * Returns 0 if the time zone is valid, 1 otherwise.
 */
int set_time_zone(const char *zone_name)
{
  setenv("TZ", zone_name, 1 /*overwrite*/);

  // Update the following **global** variables:
  //
  // * tzname[0]: the abbreviation in standard time
  // * tzname[1]: the abbreviation in DST time
  //
  // If -D _XOPEN_SOURCE is set, these globals are set:
  //
  // * timezone: the UTC offset in seconds with the opposite sign
  // * daylight: is 1 if the time zone has ever had daylight savings
  // (What a mess.)
  tzset();

  // tzset() does not set an error status. If the zone does not exist, then
  // tzset() will set the zone to UTC, so daylight offset will be 0. How do we
  // know if the requested zone exists in the database?
  //
  // The following heuristics works on Linux:
  //  * tzname[0] == a truncated version of the original zone name, and
  //  * tzname[1] == ""
  //
  // On MacOS, both "UTC" and an invalid zone returns the following:
  //  * tzname[0] == "UTC"
  //  * tzname[1] == "   "
  // so we cannot tell if the zone is valid or not.
  bool invalid = strncmp(tzname[0], zone_name, strlen(tzname[0])) == 0
      && tzname[1][0] == '\0';

  return invalid;
}

const char * const ZONES[] = {
  "UTC",
  "Etc/UCT",
  "NoWhereContinent/DoesNotExist",
  "America/Los_Angeles",
  "America/New_York",
  "Europe/London",
  "Asia/Bangkok",
};

#define NUM_ZONES (sizeof(ZONES) / sizeof(ZONES[0]))

void print_hms(long offset)
{
  char sign;
  if (offset < 0) {
    sign = '-';
    offset = -offset;
  } else {
    sign = '+';
  }
  int s = offset % 60;
  int minutes = offset / 60;
  int m = minutes % 60;
  int h = minutes / 60;
  printf("%c%02d:%02d:%02d", sign, h, m, s);
}

void print_time(time_t epoch_seconds)
{
  struct tm tms;
  localtime_r(&epoch_seconds, &tms);

  int year = tms.tm_year + 1900;
  int month = tms.tm_mon + 1;
  int day = tms.tm_mday;
  int hour = tms.tm_hour;
  int minute = tms.tm_min;
  int second = tms.tm_sec;
  long utcoff = tms.tm_gmtoff; // available with -D _GNU_SOURCE

  printf("%04d-%02d-%02dT%02d:%02d:%02d",
      year, month, day, hour, minute, second);
  print_hms(utcoff);
}

int main()
{
  //time_t epoch_seconds = 1675209600; // 2023-02-01T00:00:00 UTC
  time_t epoch_seconds = time(NULL);
  for (int i = 0; i < (int)NUM_ZONES; i++) {
    const char *name = ZONES[i];
    int err = set_time_zone(name);
    if (err) {
      printf("Zone %s: ERROR: does not exist\n", name);
      printf("\ttzname[]={\"%s\",\"%s\"}\n", tzname[0], tzname[1]);
    } else {
      printf("Zone %s: ", name);
      print_time(epoch_seconds);
      printf("\n");
      printf("\ttzname[]={\"%s\",\"%s\"}\n", tzname[0], tzname[1]);
    }
  }
}
