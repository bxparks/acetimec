/*
 * An example program intended to be compiled in an Arduino environment,
 * using the acetimec library, also compiled under an Arduino environment.
 *
 * The Makefile uses EpoxyDuino, which provides an Arduino API environment on a
 * POSIX (Linux, MacOS, FreeBSD) machine.
 */

#include <Arduino.h>
#include <acetimec.h>

// ESP32 does not define SERIAL_PORT_MONITOR
#if defined(ESP32)
  #define SERIAL_PORT_MONITOR Serial
#endif

struct AtcZoneProcessor losAngelesProcessor;

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif

  Serial.begin(115200);
  if (!Serial) {} // wait for Serial ready (Leonardo/Micro)

#if defined(EPOXY_DUINO)
  Serial.setLineModeUnix();
#endif

  atc_processor_init(&losAngelesProcessor);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &losAngelesProcessor};

  // Create a ZoneDateTime of 2019-03-10T03:00:00, just after DST shift
  struct AtcPlainDateTime localTime = {2019, 3, 10, 3, 0, 0};
  struct AtcZonedDateTime startTime;
  atc_zoned_date_time_from_plain_date_time(
      &startTime, &localTime, &tz, kAtcDisambiguateCompatible);

  // Print the AceTime epoch seconds
  SERIAL_PORT_MONITOR.print(F("Epoch Seconds: "));
  atc_time_t epochSeconds = atc_zoned_date_time_to_epoch_seconds(&startTime);
  SERIAL_PORT_MONITOR.println(epochSeconds);

  // Create a ZonedDateTime from the epoch seconds.
  struct AtcZonedDateTime lat;
  atc_zoned_date_time_from_epoch_seconds(&lat, epochSeconds, &tz);
  if (atc_zoned_date_time_is_error(&lat)) {
    SERIAL_PORT_MONITOR.println("ERROR");
    return;
  }

  // Print in human readable ISO8601 format.
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);
  atc_zoned_date_time_print(&sb, &lat);
  atc_buf_close(&sb);
  SERIAL_PORT_MONITOR.println(sb.p);

#if defined(EPOXY_DUINO)
	exit(1);
#endif
}

void loop() {}
