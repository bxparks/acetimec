/*
 * Determine the size of various components of the AceTimeC library.
 */

#include <stdint.h> // uint8_t
#include <Arduino.h>

// List of features of the AceTime library that we want to examine.
#define FEATURE_BASELINE 0
#define FEATURE_LOCAL_DATE_TIME 1
#define FEATURE_ZONED_DATE_TIME 2
#define FEATURE_TIME_ZONE 3
#define FEATURE_TIME_ZONE2 4
#define FEATURE_ZONE_REGISTRY 5
#define FEATURE_ZONE_AND_LINK_REGISTRY 6

// Select one of the FEATURE_* parameter and compile. Then look at the flash
// and RAM usage, compared to FEATURE_BASELINE usage to determine how much
// flash and RAM is consumed by the selected feature.
// NOTE: This line is modified by a 'sed' script in collect.sh. Be careful
// when modifying its format.
#define FEATURE 0

#if FEATURE != FEATURE_BASELINE
  #include <acetimec.h>
#endif

// Set this variable to prevent the compiler optimizer from removing the code
// being tested when it determines that it does nothing.
volatile int guard;

// Use this instead of a constant to prevent the compiler from calculating
// certain values (e.g. toEpochSeconds()) at compile-time.
volatile int16_t year = 2019;

// Create the various objects that we want to measure as global variables so
// that their static memory consumption is detected. The previous version placed
// all these inside the setup() method, which creates the objects on the stack,
// which do not get detected as memory consumption, so don't show up in the
// *.txt files.
#if FEATURE == FEATURE_LOCAL_DATE_TIME
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
#elif FEATURE == FEATURE_ZONED_DATE_TIME
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
  AtcZonedDateTime zdt;
#elif FEATURE == FEATURE_TIME_ZONE
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
  AtcZoneProcessor processor;
  AtcTimeZone tz = {&kAtcZoneAmerica_Los_Angeles, &processor};
  AtcZonedDateTime zdt;
#elif FEATURE == FEATURE_TIME_ZONE2
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
  AtcZoneProcessor processor1;
  AtcZoneProcessor processor2;
  AtcTimeZone tz1 = {&kAtcZoneAmerica_Los_Angeles, &processor1};
  AtcTimeZone tz2 = {&kAtcZoneAmerica_New_York, &processor2};
  AtcZonedDateTime zdt1;
  AtcZonedDateTime zdt2;
#elif FEATURE == FEATURE_ZONE_REGISTRY
  AtcZoneRegistrar registrar;
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
  AtcZoneProcessor processor;
  AtcZonedDateTime zdt;
#elif FEATURE == FEATURE_ZONE_AND_LINK_REGISTRY
  AtcZoneRegistrar registrar;
  AtcLocalDateTime ldt = {year, 6, 17, 9, 18, 0, 0 /*fold*/};
  AtcZoneProcessor processor;
  AtcZonedDateTime zdt;

#endif

// TeensyDuino seems to pull in malloc() and free() when a class with virtual
// functions is used polymorphically. This causes the memory consumption of
// FEATURE_BASELINE (which normally has no classes defined, so does not include
// malloc() and free()) to be artificially small which throws off the memory
// consumption calculations for all subsequent features. Let's define a
// throw-away class and call its method for all FEATURES, including BASELINE.
#if defined(TEENSYDUINO)
  class FooClass {
    public:
      virtual void doit() {
        guard = 0;
      }
  };

  FooClass* foo;
#endif

void setup() {
#if defined(TEENSYDUINO)
  // Force Teensy to bring in malloc(), free() and other things for virtual
  // dispatch.
  foo = new FooClass();
#endif

#if FEATURE == FEATURE_BASELINE
  guard = 0;
#elif FEATURE == FEATURE_LOCAL_DATE_TIME
  atc_time_t epoch_seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  guard ^= epoch_seconds;
#elif FEATURE == FEATURE_ZONED_DATE_TIME
  int8_t err = atc_zoned_date_time_from_local_date_time(
      &zdt, &ldt, &atc_time_zone_utc);
  if (err) return;
  atc_time_t epoch_seconds = atc_local_date_time_to_epoch_seconds(&ldt);
  guard ^= epoch_seconds;
#elif FEATURE == FEATURE_TIME_ZONE
  atc_processor_init(&processor);
  int8_t err = atc_zoned_date_time_from_local_date_time(&zdt, &ldt, &tz);
  if (err) return;
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
  guard ^= epoch_seconds;
#elif FEATURE == FEATURE_TIME_ZONE2
  atc_processor_init(&processor1);
  atc_processor_init(&processor2);
  int8_t err = atc_zoned_date_time_from_local_date_time(&zdt1, &ldt, &tz1);
  if (err) return;
  err = atc_zoned_date_time_from_local_date_time(&zdt2, &ldt, &tz2);
  atc_zoned_date_time_convert(&zdt1, &tz2, &zdt2);
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt1);
  guard ^= epoch_seconds;
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt2);
  guard ^= epoch_seconds;
#elif FEATURE == FEATURE_ZONE_REGISTRY
  atc_processor_init(&processor);
  atc_registrar_init(&registrar, kAtcZoneRegistry, kAtcZoneRegistrySize);
  const AtcZoneInfo *zone_info = atc_registrar_find_by_name(
    &registrar, "America/Los_Angeles");
  AtcTimeZone tz = {zone_info, &processor};
  int8_t err = atc_zoned_date_time_from_local_date_time(&zdt, &ldt, &tz);
  if (err) return;
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
  guard ^= epoch_seconds;

#elif FEATURE == FEATURE_ZONE_AND_LINK_REGISTRY
  atc_processor_init(&processor);
  atc_registrar_init(&registrar,
      kAtcZoneAndLinkRegistry, kAtcZoneAndLinkRegistrySize);
  const AtcZoneInfo *zone_info = atc_registrar_find_by_name(
    &registrar, "America/Los_Angeles");
  AtcTimeZone tz = {zone_info, &processor};
  int8_t err = atc_zoned_date_time_from_local_date_time(&zdt, &ldt, &tz);
  if (err) return;
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
  guard ^= epoch_seconds;

#else
  #error Unknown FEATURE
#endif

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}