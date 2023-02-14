/*
 * A program to compare the DST transitions calculated by AceTimeC with the
 * standard C library.
 */
#include <stdio.h> // printf()
#include <stdlib.h> // setenv()
#include <string.h> //strncmp()
#include <time.h> // localtime_r()
#include <stdlib.h> // setenv()
#include <stdbool.h> // bool
#include <unistd.h> // sleep()
#include <acetimec.h> // AceTimeC functions

//-----------------------------------------------------------------------------

/**
 * Set the timezone as currently specified by the "TZ" environment variable.
 * Returns 0 if the time zone is valid, 1 otherwise.
 */
uint8_t set_time_zone(const char *zone_name) {
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

  return invalid;
}

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

//-----------------------------------------------------------------------------

//AtcZoneRegistrar registrar;
AtcZoneProcessor processor;
const int16_t start_year = 2000;
const int16_t until_year = 2100;

void setup()
{
  printf("==== validate_against_libc()\n");
  printf("start_year: %d\n", start_year);
  printf("until_year: %d\n", until_year);

  //atc_registrar_init(&registrar, kAtcZoneRegistry, kAtcZoneRegistrySize);
  atc_processor_init(&processor);
  atc_set_current_epoch_year(2050); // 2050 is the default
}

/** Check that the zones and links in AceTimeC are avaiable in libc. */
int check_zone_names()
{
  printf("==== check_zone_names()\n");

  int err = 0;

  // Check a random zone name returns not-ok
  const char doesnotexist[] = "Random/String";
  {
    int local_err = set_time_zone(doesnotexist);
    if (! local_err) {
      printf("ERROR: libc time should have returned error for %s\n",
            doesnotexist);
    }
    err |= local_err;
  }

  // Check all the zones and links in the AceTimeC zonedb registry.
  printf("Checking %d Zones and Links\n", kAtcZoneAndLinkRegistrySize);
  for (int i = 0; i < kAtcZoneAndLinkRegistrySize; i++) {
    const AtcZoneInfo *info = kAtcZoneAndLinkRegistry[i];
    // Check that the zone name is supported by the current libc.
    int local_err = set_time_zone(info->name);
    if (local_err) {
      printf("ERROR: libc time does not suport Zone %s\n", info->name);
    }
    err |= local_err;
  }

  return err;
}

//-----------------------------------------------------------------------------

int check_epoch_seconds(const AtcTimeZone *tz, atc_time_t epoch_seconds)
{
  // Convert epoch seconds to ZonedDateTime using AceTimeC
  struct AtcZonedDateTime zdt;
  int8_t err = atc_zoned_date_time_from_epoch_seconds(&zdt, epoch_seconds, tz);
  if (err) {
    printf("ERROR: Zone %s: epoch seconds=%d: "
        "unable to create AtcZoneDateTime\n",
        tz->zone_info->name, epoch_seconds);
    return err;
  }

  // Convert epoch seconds to date-time components using C libc.
  struct tm tms;
  time_t unix_seconds = atc_convert_to_unix_seconds(epoch_seconds);
  localtime_r(&unix_seconds, &tms);
  int year = tms.tm_year + 1900;
  int month = tms.tm_mon + 1;
  int day = tms.tm_mday;
  int hour = tms.tm_hour;
  int minute = tms.tm_min;
  int second = tms.tm_sec;
  long offset = tms.tm_gmtoff;

  // Verify that they are same.
  if (year != zdt.year) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched year (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, year, zdt.year);
    return kAtcErrGeneric;
  }
  if (month != zdt.month) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched month (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, month, zdt.month);
    return kAtcErrGeneric;
  }
  if (day != zdt.day) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched day (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, day, zdt.day);
    return kAtcErrGeneric;
  }
  if (hour != zdt.hour) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched hour (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, hour, zdt.hour);
    return kAtcErrGeneric;
  }
  if (minute != zdt.minute) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched minute (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, minute, zdt.minute);
    return kAtcErrGeneric;
  }
  if (second != zdt.second) {
    printf("ERROR: Zone %s: epoch_seconds=%d; mismatched second (%d != %d)\n",
        tz->zone_info->name, epoch_seconds, second, zdt.second);
    return kAtcErrGeneric;
  }
  if (offset != (long) zdt.offset_minutes * 60) {
    printf("ERROR: Zone %s: epoch_seconds=%d; "
        "mismatched UTC offset (%ld != %ld)\n",
        tz->zone_info->name, epoch_seconds,
        offset, (long) zdt.offset_minutes * 60);
    return kAtcErrGeneric;
  }
  return kAtcErrOk;
}

int check_transitions(const AtcTimeZone *tz)
{
  set_time_zone(tz->zone_info->name);
  atc_processor_init_for_zone_info(tz->zone_processor, tz->zone_info);

  int err = 0;
  int num_transitions = 0;
  for (int16_t year = start_year; year < until_year; ++year) {
    atc_processor_init_for_year(tz->zone_processor, year);

    struct AtcTransitionStorage *ts = &tz->zone_processor->transition_storage;
    struct AtcTransition **begin =
        atc_transition_storage_get_active_pool_begin(ts);
    struct AtcTransition **end =
        atc_transition_storage_get_active_pool_end(ts);
    for (struct AtcTransition **t = begin; t != end; ++t) {
      // Skip if start year of transition does not match the current. This can
      // happen because we generate transitions over a 14-month interval
      // spanning the current year.
      struct AtcDateTuple *start = &((*t)->start_dt);
      if (start->year != year) continue;

      // Skip if the UTC year bleeds under or over the boundaries.
      if ((*t)->transition_time_u.year < start_year) continue;
      if ((*t)->transition_time_u.year >= until_year) continue;

      num_transitions++;
      atc_time_t epoch_seconds = (*t)->start_epoch_seconds;

      // Add a test data just before the transition
      err |= check_epoch_seconds(tz, epoch_seconds - 1);
      if (err) continue;

      // Add a test data at the transition itself (which will
      // normally be shifted forward or backwards).
      err |= check_epoch_seconds(tz, epoch_seconds);
      if (err) continue;
    }
  }
  printf("Transitions: %d; ", num_transitions);

  return err;
}

// Check sampled dates from start year to until year.
int check_samples(const AtcTimeZone *tz)
{
  int num_samples = 0;
  for (int16_t year = start_year; year < until_year; year++) {
    for (uint8_t month = 1; month <=12; month++) {
      for (uint8_t day = 1; day <=28; day++) {
        AtcZonedDateTime zdt;
        AtcLocalDateTime ldt = {year, month, day, 2, 0, 0, 0 /*fold*/};

        int err = atc_zoned_date_time_from_local_date_time(&zdt, &ldt, tz);
        if (err) {
          char s[64];
          AtcStringBuffer buf;
          atc_buf_init(&buf, s, sizeof(s));
          atc_local_date_time_print(&ldt, &buf);
          atc_buf_close(&buf);
          printf("ERROR: Zone %s: unable to create AtcZoneDateTime for %s\n",
              tz->zone_info->name, buf.p);
          return err;
        }

        num_samples++;
        atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
        check_epoch_seconds(tz, epoch_seconds);
      }
    }
  }
  printf("Samples: %d\n", num_samples);
  return kAtcErrOk;
}

// Check the DST transitions as determined by AceTimeC.
int check_date_components()
{
  printf("==== check_date_components()\n");

  int err = 0;
  for (int i = 0; i < kAtcZoneAndLinkRegistrySize; i++) {
    const AtcZoneInfo *info = kAtcZoneAndLinkRegistry[i];
    printf("%d: Zone %s: ", i, info->name);
    AtcTimeZone tz = {info, &processor};

    err |= check_transitions(&tz);
    err |= check_samples(&tz);
  }
  return err;
}

int main()
{
  setup();
  int err = check_zone_names();
  err |= check_date_components();
  return err;
}
