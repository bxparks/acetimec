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
#include <unistd.h> // sleep

/**
 * Set the timezone as currently specified by the "TZ" environment variable.
 * Returns true if the time zone is valid, false otherwise.
 */
bool set_time_zone(const char *zone_name) {
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

  // tzset() does not set an error status, so we don't know if the ZONE_NAME is
  // valid or not. So we use the following heuristics: If the zone does not
  // exist, then tzset() will set the zone to UTC, so daylight offset will be
  // 0. But there are legitimate timezones which track UTC. But when the zone
  // is invalid, it seems like tzname[0] is set to a truncated version of the
  // original zone name, and tzname[1] is set to an empty string.
  bool invalid = strncmp(tzname[0], zone_name, strlen(tzname[0])) == 0
      && tzname[1][0] == '\0';

  return !invalid;
}

const char * const ZONES[] ={
  "UTC",
  "America/Los_Angeles",
  "America/New_York",
  "Europe/London",
  "Asia/Bangkok",
};

#define NUM_ZONES (sizeof(ZONES) / sizeof(ZONES[0]))

void print_time(time_t epoch_seconds) {
  struct tm tms;
  localtime_r(&epoch_seconds, &tms);

  int year = tms.tm_year + 1900;
  int month = tms.tm_mon + 1;
  int day = tms.tm_mday;
  int hour = tms.tm_hour;
  int minute = tms.tm_min;
  int second = tms.tm_sec;
  //int gmtoff = tms.tm_gmtoff;

  printf("%04d-%02d-%02dT%02d:%02d:%02d\n",
      year, month, day, hour, minute, second);
}

int main() {

  for (int n = 0; n < 1000; n++) {
    //time_t epoch_seconds = 1675209600; // 2023-02-01T00:00:00 UTC
    time_t epoch_seconds = time(NULL);
    for (int i = 0; i < (int)NUM_ZONES; i++) {
      const char *name = ZONES[i];
      bool ok = set_time_zone(name);
      if (ok) {
        printf("Zone %s: ", name);
        print_time(epoch_seconds);
      } else {
        printf("ERROR: Invalid zone %s\n", name);
      }
    }
    printf("\n");
    sleep(2);
  }
}

