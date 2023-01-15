/*
 * An example program intended to be compiled in an Arduino environment,
 * using the AceTimeC library, also compiled under an Arduino environment.
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

struct AtcZoneProcessing losAngelesProcessing;

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif

  Serial.begin(115200);
  if (!Serial) {} // wait for Serial ready (Leonardo/Micro)

#if defined(EPOXY_DUINO)                                                        
  Serial.setLineModeUnix();                                                     
#endif

  atc_processing_init(&losAngelesProcessing);
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &losAngelesProcessing};

  // Create a ZoneDateTime of 2019-03-10T03:00:00, just after DST shift
  struct AtcLocalDateTime localTime = { 2019, 3, 10, 3, 0, 0 };
  struct AtcZonedDateTime startTime;
  atc_zoned_date_time_from_local_date_time(
      &startTime, &localTime, 0 /*fold*/, tz);

  // Print the AceTime epoch seconds
  SERIAL_PORT_MONITOR.print(F("Epoch Seconds: "));
  atc_time_t epochSeconds = atc_zoned_date_time_to_epoch_seconds(&startTime);
  SERIAL_PORT_MONITOR.println(epochSeconds);

  // Create a ZonedDateTime from the epoch seconds.
  struct AtcZonedDateTime lat;
  int8_t err = atc_zoned_date_time_from_epoch_seconds(
      &lat, epochSeconds, tz);
  if (err) {
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
