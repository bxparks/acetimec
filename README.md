# AceTime for C

[![ACUnit Tests](https://github.com/bxparks/acetimec/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/bxparks/acetimec/actions/workflows/unit_tests.yml)

The `acetimec` library is a date and time zone library for the C language, based
on algorithms and techniques from the
[AceTime](https://github.com/bxparks/AceTime) library for the Arduino
environment. Just like AceTime, this library supports all time zones defined by
the [IANA TZ database](https://www.iana.org/time-zones).

The functionality provided by acetimec is a subset of the AceTime, mostly
because the C language does not provide the same level of abstraction and
encapsulation of the C++ language. If the equivalent functionality of AceTime
was attempted in this library, the public API would become too large and
complex, with diminishing returns from the increased complexity. I decided that
this library would implement the only algorithms provided by the
`ExtendedZoneProcessor` class of the AceTime library. It does not implement the
functionality provided by the `BasicZoneProcessor` of the AceTime library.

Due to time constraints, this README document provides only a small fraction of
the documentation provided by the `README.md` and `USER_GUIDE.md` documents of
the AceTime library. If you need more detailed information, please consult those
latter documents.

**Status**: Alpha-level software, not ready for public consumption.

**Version**: 0.10.0 (2023-05-19, TZDB version 2023c)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Table of Contents

* [Example](#Example)
* [Installation](#Installation)
* [Usage](#Usage)
    * [Header File](#HeaderFile)
    * [Constants](#Constants)
    * [atc_time_t](#AtcTimeT)
    * [Epoch](#Epoch)
    * [LocalDate](#LocalDate)
    * [LocalTime](#LocalTime)
    * [AtcLocalDateTime](#AtcLocalDateTime)
    * [AtcOffsetDateTime](#AtcOffsetDateTime)
    * [AtcZonedDateTime](#AtcZonedDateTime)
    * [AtcTimeZone](#AtcTimeZone)
    * [AtcZoneProcessor](#AtcZoneProcessor)
    * [AtcZoneInfo](#AtcZoneInfo)
    * [Zone Database and Registry](#ZoneDatabaseAndRegistry)
    * [AtcZonedExtra](#AtcZonedExtra)
    * [AtcZoneRegistrar](#AtcZoneRegistrar)
* [Bugs and Limitations](#Bugs)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Example"></a>
## Example

The expected usage is something like this:

```C
#include <stdio.h>
#include <acetimec.h>

AtcZoneProcessor processor_la; // Los Angeles
AtcZoneProcessor processor_ny; // New York

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
  if (atc_zoned_date_time_is_error(&zdtla)) { ... }

  // Print the date for Los Angeles.
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Convert zoned_date_time to back to epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { ... }
  if (seconds != epoch_seconds) { ... }
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
  if (atc_zoned_date_time_is_error(&zdtla)) { ... }

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
  atc_zoned_date_time_convert(&zdtla, tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtla)) { ... }

  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, zdtny);
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
```

The complete example at [examples/hello_acetime.c](examples/hello_acetimec/)
prints the following:

```
======== ZonedDateTime from epoch seconds
Epoch Seconds: 3432423
Los Angeles: 2050-02-09T09:27:03-08:00[America/Los_Angeles]
Converted Seconds: 3432423
======== ZonedDateTime from LocalDateTime
LocalDateTime: 2022-11-06T01:30:00
fold: 1
Los Angeles: 2022-11-06T01:30:00-08:00[America/Los_Angeles]
Epoch Seconds: -856881000
======== ZonedDateTime to different time zone
New York: 2022-11-06T04:30:00-05:00[America/New_York]
Epoch Seconds: -856881000
```

<a name="Installation"></a>
## Installation

I am not familiar with any of the C language package managers. To obtain this
library, you should manually clone the project into an appropriate place on your
computer:

```
$ git clone https://github.com/bxparks/acetimec
```

There are many different ways that a C library can be incorporated into an
existing project, depending the platform. I do almost all my development on a
Linux box, so I provide the following infrastructure:

* The `./src/Makefile` provides a target `$ make acetimec.a` which compiles the
  `*.c` files to `*.o` files, then collects these object files into a static
  library named `./src/acetimec.a`.
* Downstream applications can link to this static library. This can be
  done by passing the `./src/acetimec.a` file to the linker.
* Downstream application source code needs to include the `./src/acetimec.h`
  header file. This can be done using the `-I` flag to the compiler.

<a name="Usage"></a>
## Usage

<a name="HeaderFile"></a>
### Header File

The header file must be included like this:

```C
#include <acetimec.h>
```

<a name="Constants"></a>
### Constants

A number of constants are provided by this library:

* `kAtcErrOk` (0)
    * indicates success of a function
* `kAtcErrGeneric` (1)
    * indicates a generic failure of a function
* `kAtcInvalidYear`
    * `INT16_MIN` (-32736)
    * indicates an invalid year
* `kAtcInvalidEpochSeconds`
    * `INT32_MIN` (-2147483648)
    * indicates an invalid epoch seconds
* ISO Weekdays
    * `kAtcIsoWeekdayMonday` (1)
    * `kAtcIsoWeekdayTuesday`
    * `kAtcIsoWeekdayWednesday`
    * `kAtcIsoWeekdayThursday`
    * `kAtcIsoWeekdayFriday`
    * `kAtcIsoWeekdaySaturday`
    * `kAtcIsoWeekdaySunday` (7)

<a name="AtcTimeT"></a>
### `atc_time_t`

The library defines an `atc_time_t` type which is analogous to the normal
`time_t` from the standard C library. (The POSIX standard reserves all symbols
which end with `_t`. But the `atc_time_t` is so close to the `time_t` type that
I felt that it would be more confusing to name it something like `AtcTimeT`.)

```C
typedef int32_t atc_time_t;
```

It is a signed, 32-bit integer that counts the number of POSIX seconds from the
epoch of this library. That epoch will normally be 2050-01-01 00:00:00 UTC,
instead of the POSIX standard of 1970-01-01 00:00:00 UTC. That means that
largest date that can be represented by `atc_time_t` is 2118-01-20 03:14:07 UTC.

The current epoch year can be changed using the `atc_set_current_epoch_year()`
as described in the next section.

<a name="Epoch"></a>
### Epoch

The functions in [epoch.h](src/acetimec/epoch.h) provide features related to
the epoch used by the acetimec library. By default, the epoch is 2050-01-01
00:00:00 UTC, which allows the 32-bit `ace_time_t` type to support dates from
the year 2000 until the year 2100, at a minimum. However, unlike most timezone
libraries, the epoch year can be changed at runtime so that the `ace_time_t` can
be used to support any dates within approximately +/- 50 years of the epoch
year.

The following functions are used to get and set the epoch year:

* `int16_t atc_get_current_epoch_year(void)`
    * Returns the current epoch year.
* `void atc_set_current_epoch_year(int16_t year)`
    * Sets the current epoch year to `year`.

**Warning**: If the epoch year is changed using the
`atc_set_current_epoch_year()` function, then the `atc_processor_init()`
function (see [AtcZoneProcessor](#AtcZoneProcessor)) must be called to
reinitialize any instance of `AtcZoneProcessor` that may have used a different
epoch year.

The following convenience functions return the range of validity of the
`ace_time_t` type:

* `int16_t atc_epoch_valid_year_lower(void)`
    * Returns the lower bound of the year that can be represented by
      `ace_time_t`.
    * This currently returns `epoch_year - 50` as a conservative estimate.
    * The actual lower bound is 10-15 years higher, and a future version of the
      library may update the value returned by this function.
* `int16_t atc_epoch_valid_year_upper(void)`
    * Returns the upper bound of the year that can be represented by
      `ace_time_t`.
    * This currently returns `epoch_year + 50` as a conservative estimate.
    * The actual upper bound is 10-15 years higher, and a future version of the
      library may update the value returned by this function.

The following are low level internal functions that convert a given `(year,
month, day)` triple in the proleptic Gregorian calendar to the number of days
from an arbitrary, but fixed, internal epoch date (currently the year 2000).
They are used as the basis for converting the Gregorian date to the number of
offset days from the user-adjustable epoch year. They are not expected to be
used by client applications.

* `int32_t atc_convert_to_days(int16_t year, uint8_t month, uint8_t day)`
* `void atc_convert_from_days(int16_t days, uint8_t *year, uint8_t *month,
   uint8_t *day)`

<a name="LocalDate"></a>
### LocalDate

The functions in [local_date.h](src/acetimec/local_date.h) provide features
related to the Gregorian `(year, month, day)` triple. These functions do not
know about the time components `(hour, minute, second)` or the timezone. They
often represent either the local date, or the UTC date, depending on context.

* `bool atc_is_leap_year(int16_t year)`
    * Returns `true` if the given `year` is a leap year in the Gregorian
      calendar.
* `uint8_t atc_local_date_days_in_year_month(int16_t year, uint8_t month)`
    * Returns the number of days in the given `(year, month)` pair.

The `atc_local_date_day_of_week()` returns the ISO day of week of the given
`(year, month, day)` date. The ISO weekday starts with Monday as 1, and ending
with Sunday as 7:

```C
enum {
  kAtcIsoWeekdayMonday = 1,
  kAtcIsoWeekdayTuesday,
  kAtcIsoWeekdayWednesday,
  kAtcIsoWeekdayThursday,
  kAtcIsoWeekdayFriday,
  kAtcIsoWeekdaySaturday,
  kAtcIsoWeekdaySunday,
};

uint8_t atc_local_date_day_of_week(int16_t year, uint8_t month, uint8_t day)`
```

The following 2 functions convert the Gregorian date to and from the number of
days from the current epoch (given by `atc_get_current_epoch_year()`). These
functions should return the correct value for any year in the range of `0 < year
< 10000`, but no validation is performed for invalid dates. For example, the
behavior is undefined for Feb 30 which is an invalid date.

```C
int32_t atc_local_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day);

void atc_local_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day);
```

The following functions increment and decrement the date by one day:

```C
void atc_local_date_increment_one_day(
    int16_t *year, uint8_t *month, uint8_t *day);

void atc_local_date_decrement_one_day(
    int16_t *year, uint8_t *month, uint8_t *day);
```

<a name="LocalTime"></a>
### LocalTime

The `local_time.h` file contains functions related to the local `(hour, minute,
second)`. Currently a single function is provided:

```C
int32_t atc_local_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second);
```

<a name="AtcLocalDateTime"></a>
### AtcLocalDateTime

The functions in [local_date_time.h](src/acetimec/local_date_time.h) operate
on the `AtcLocalDateTime` type which represents the wall date and time, without
reference to a time zone:

```C
typedef struct AtcLocalDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;
} AtcLocalDateTime;
```

Here are the functions which operate on this data type:

```C
void atc_local_date_time_set_error(AtcLocalDateTime *ldt);

bool atc_local_date_time_is_error(const AtcLocalDateTime *ldt);

atc_time_t atc_local_date_time_to_epoch_seconds( const AtcLocalDateTime *ldt);

void atc_local_date_time_from_epoch_seconds(
    AtcLocalDateTime *ldt,
    atc_time_t epoch_seconds);

int64_t atc_local_date_time_to_unix_seconds( const AtcLocalDateTime *ldt);

void atc_local_date_time_from_unix_seconds(
    AtcLocalDateTime *ldt,
    int64_t unix_seconds);

void atc_local_date_time_print(
    AtcStringBuffer *sb,
    const AtcLocalDateTime *ldt);
```

The `atc_local_date_time_set_error(ldt)` marks the given `ldt` as invalid. This
causes `atc_local_date_time_is_error(ldt)` to return `true`.

The `atc_local_date_time_to_epoch_seconds()` function converts the given
`AtcLocalDateTime` into its `atc_time_t` epoch seconds. If an error occurs, the
function returns `kAtcInvalidEpochSeconds`.

The `atc_local_date_time_from_epoch_seconds()` function converts the given epoch
seconds into the `AtcLocalDateTime` components. If an error occurs, the `ldt` is
set to its error value.

The `fold` parameter is both an input parameter and an output parameter, and has
the meaning as the `fold` parameter in the AceTime library, which borrowed the
concept from the [PEP 495](https://www.python.org/dev/peps/pep-0495/) document
in Python 3.6.

The `fold` as an output parameter is used to disambiguate a time which
occurs twice due to a DST change. For example, in most of North America, the
time zone switches from DST to Standard time in the fall. The wall clock "falls
back" from 02:00 to 01:00, which means that the time from 01:00 to 02:00 occurs
twice. The `fold` parameter is returned as 0 for the first occurrence, and 1 for
the second occurrence.

The `fold` as an input parameter is used to specify the UTC offset to be used
when converting a LocalDateTime to an `epoch_seconds` around a DST gap, for
example, when the clock jumps from 2:00 to 03:00 in North America. If `fold=0`,
the given LocalDateTime is interpreted using the UTC offset *before* the gap,
which then normalizes to a ZonedDateTime after the gap. If `fold=1`, the given
LocalDateTime is interpreted using the UTC offset *after* the gap, which then
normalizes to a ZonedDateTime before the gap.

<a name="AtcOffsetDateTime"></a>
### AtcOffsetDateTime

The functions in [offset_date_time.h](src/acetimec/offset_date_time.h) operate
on the `AtcOffsetDateTime` type which represents a date-time with a fixed offset
from UTC:

```C
typedef struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int32_t offset_seconds;
} AtcOffsetDateTime;
```

The memory layout of `AtcOffsetDateTime` was designed to be identical to
`AtcLocalDateTime` so that functions that accept a pointer to `AtcLocalDateTime`
can be given a pointer to `AtcOffsetDateTime` as well.

Here are the functions that operate on the `AtcOffsetDateTime` object:

```C
void atc_offset_date_time_set_error(AtcOffsetDateTime *odt);

bool atc_offset_date_time_is_error(const AtcOffsetDateTime *odt);

atc_time_t atc_offset_date_time_to_epoch_seconds(const AtcOffsetDateTime *odt);

void atc_offset_date_time_from_epoch_seconds(
    AtcOffsetDateTime *odt,
    atc_time_t epoch_seconds,
    int32_t offset_seconds);

int64_t atc_offset_date_time_to_unix_seconds(const AtcOffsetDateTime *odt);

void atc_offset_date_time_from_unix_seconds(
    AtcOffsetDateTime *odt,
    int64_t unix_seconds,
    int32_t offset_seconds);

void atc_offset_date_time_print(
    AtcStringBuffer *sb,
    const AtcOffsetDateTime *odt);
```

The `atc_offset_date_time_set_error(odt)` marks the given `odt` as invalid .
This causes `atc_offset_date_time_is_error(odt)` to return `true`.

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`AtcOffsetDateTime` into its `atc_time_t` epoch seconds, taking into account the
`offset_seconds` field. If an error occurs, the function returns
`kAtcInvalidEpochSeconds`. The `fold` parameter of the input `AtcOffsetDateTime`
is ignored because the `odt.offset_seconds` field is sufficient to disambiguate
multiple instances.

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`epoch_seconds` and `offset_seconds` into the `AtcOffsetDateTime` components. If
an error occurs, the function returns `kAtcErrGeneric`, otherwise it returns
`kAtcErrOk`. The `odt.fold` parameter will always be set to 0.

<a name="AtcZonedDateTime"></a>
### AtcZonedDateTime

The functions in [zoned_date_time.h](src/acetimec/zoned_date_time.h) operate
on the `AtcZonedDateTime` data structure, which is identical to the
`AtcOffsetDateTime` data structure with the addition of a reference to the TZDB
time zone:

```C
typedef struct AtcZonedDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int32_t offset_seconds; /* possibly ignored */
  AtcTimeZone tz;
} AtcZonedDateTime;
```

The memory layout of `AtcZonedDateTime` was designed to be identical to
`AtcOffsetDateTime`, so that functions that accept a pointer to
`AtcOffsetDateTime` can also accept pointers to `AtcZonedDateTime`.

The following functions operate on the `AtcZonedDateTime` struct:

```C
void atc_zoned_date_time_set_error(AtcZonedDateTime *zdt);

bool atc_zoned_date_time_is_error(const AtcZonedDateTime *zdt);

atc_time_t atc_zoned_date_time_to_epoch_seconds(const AtcZonedDateTime *zdt);

void atc_zoned_date_time_from_epoch_seconds(
    AtcZonedDateTime *zdt,
    atc_time_t epoch_seconds,
    AtcTimeZone *tz);

int64_t atc_zoned_date_time_to_unix_seconds(const AtcZonedDateTime *zdt);

void atc_zoned_date_time_from_unix_seconds(
    AtcZonedDateTime *zdt,
    int64_t unix_seconds,
    AtcTimeZone *tz);

void atc_zoned_date_time_from_local_date_time(
    AtcZonedDateTime *zdt,
    const AtcLocalDateTime *ldt,
    AtcTimeZone *tz);

void atc_zoned_date_time_convert(
    const AtcZonedDateTime *from,
    AtcTimeZone to_tz,
    AtcZonedDateTime *to);

void atc_zoned_date_time_normalize(
    AtcZonedDateTime *zdt);

void atc_zoned_date_time_print(
    AtcStringBuffer *sb,
    const AtcZonedDateTime *zdt);
```

* `atc_offset_date_time_set_error(zdt)`
    * marks the given `zdt` as invalid
    * causes `atc_offset_date_time_is_error(zdt)` to return `true`
* `atc_offset_date_time_from_epoch_seconds()`
    * Converts the given `epoch_seconds` and `tz` into the `AtcZonedDateTime`
      components.
    * If an error occurs, the function returns `kAtcErrGeneric`, otherwise it
      returns `kAtcErrOk`.
    * `AtcZonedDateTime.fold` is an *output* parameter in this function:
        * Will usually be 0 except during a DST overlap.
        * `fold=0` indicates the first occurrence of the local wall clock
        * `fold=1` indicates the second occurrence of the local wall clock.
* `atc_zoned_date_time_to_epoch_seconds()`
    * Converts the given `AtcZonedDateTime` into its `atc_time_t` epoch seconds,
      taking into account the time zone defined by the `tz` field inside the
      `AtcZonedDatetime`.
    * If an error occurs, the function returns `kAtcInvalidEpochSeconds`.
* `atc_zoned_date_time_from_local_date_time()`
    * Converts the local wall clock defined by `AtcLocalDateTime` to the
      `AtcZonedDateTime`, taking into account the time zone defined by `tz`.
    * `AtcLocalDateTime.fold` is an *input* parameter for this function.
        * In most cases, the `fold` parameter has no effect.
        * During an overlap:
            * `fold=0` indicates the earlier of the 2 repeated time,
            * `fold=1` indicates the later of the 2 repeated time.
        * During a gap:
            * `fold=0` indicates that the *earlier* UTC offset should be used,
              which causes the effective epoch seconds to be the *later* one,
              which then gets normalized to the *later* `AtcZonedDateTime`.
            * `fold=1` indicates that the *later* UTC offset should be used,
              which causes the effective epoch seconds to be the *earlier* one,
              which then gets normalized to the *earlier* `AtcZonedDateTime`.
* `atc_zoned_date_time_convert()`
    * Converts an `AtcZonedDateTime` instance from one time zone to another. The
      `src` instance contains the original time zone. The `dst` instance will
      contain the date-time of the time zone represented by `dst_zone_info`.

The conventions for the `fold` parameter are intended to be identical to the one
described by the Python [PEP 495](https://www.python.org/dev/peps/pep-0495/)
document.

<a name="AtcTimeZone"></a>
### AtcTimeZone

The `AtcTimeZone` structure represents a time zone from the IANA TZ database. It
consists of a pair of pointers, an `AtcZoneInfo*` pointer and an
`AtcZoneProcessor*` pointer, like this:

```C++
typedef struct AtcTimeZone {
  const AtcZoneInfo *zone_info;
  AtcZoneProcessor *zone_processor;
} AtcTimeZone;
```

Instances of `AtcTimeZone` are expected to be passed around by value into
functions which need to be provided a time zone.

<a name="AtcZoneProcessor"></a>
### AtcZoneProcessor

The `AtcZoneProcessor` data structure provides a workspace for the various
internal functions that perform time zone calculations. The internal details
should be considered to be private and subject to change without notice. One of
this data type should be created statically for each time zone used by the
downstream application. (Another possibility is to create one on the heap at
startup time, then never freed.)

Each time zone should be assigned an instance of the `AtcZoneProcessor`. An
instance of `AtcZoneProcessor` should be initialized only once, usually at the
beginning of the application:

```C
AtcZoneProcessor los_angeles_processor;

void setup()
{
  atc_processor_init(&los_angeles_processor);
}
```

The `AtcZoneProcessor` instance keeps a cache of UTC offset transitions
spanning a year. Multiple calls to various `atc_zoned_date_time_XXX()` functions
with the same `AtcZoneProcessor` instance within a given year will execute much
faster than other years.

If memory is tight, an `AtcZoneProcessor` instance could be used by multiple
time zones (i.e. different `AtcZoneInfo`). However, each time the time zone
changes, the internal cache of the `AtcZoneProcessor` instance will be cleared
and recalculated, so the execution speed may decrease significantly.

**Warning**: If the epoch year is changed using the
`atc_set_current_epoch_year()` function (see [Epoch](#Epoch)), then the
`atc_processor_init()` function must be called to reinitialize any instance of
`AtcZoneProcessor` that may have used a different epoch year.

<a name="AtcZoneInfo"></a>
### AtcZoneInfo

The `AtcZoneInfo` data structure in [zone_info.h](src/acetimec/zone_info.h)
defines the DST transition rules of a single time zone. The pointer to the
`AtcZoneInfo` is meant to be passed around as opaque object for the most part
since most of the fields are meant for internal consumption. There are 3
accessor functions which may be useful for end-users:

```C
bool atc_zone_info_is_link(const AtcZoneInfo *info);

const char *atc_zone_info_zone_name(const AtcZoneInfo *info);

const char *atc_zone_info_short_name(const AtcZoneInfo *info);
```

The `atc_zone_info_is_link()` function can determine whether a particular `info`
instance is a Zone entry or a Link entry.

The `atc_zone_info_zone_name()` function returns the full zone name of the
`info` instance. For example, calling this on `kAtcZoneAmerica_Los_Angeles`
returns the string `"America/Los_Angeles"`. This is useful if the `info`
instance was retrieved from the zone registrar using a zone ID instead of the
zone string.

The `atc_zone_info_short_name()` function is similar to the
`atc_zone_info_zone_name()` except that it returns the "short" name of the zone,
which is defined to be the string just after the last `/` character in the zone
name. For example, the short name of `"America/Los_Angeles"` is `"Los_Angeles"`.

<a name="ZoneDatabaseAndRegistry"></a>
### Zone Database and Registry

There are 3 zoneinfo databases provided by this library and available through
the default `<acetimec.h>` header file:

* [src/zonedb](src/zonedb)
    * All TZDB zones, from the year 2000 to 10000.
    * Exported identifiers:
        * `kAtcZoneContext`
        * `kAtcZoneXxx` (e.g. `kAtcZoneAmerica_Los_Angeles`)
        * `kAtcZoneIdXxx` (e.g. `kAtcZoneIdAmerica_Los_Angeles`)
* [src/zonedball](src/zonedball)
    * All TZDB zones, from the year 1800 to 10000.
    * The earliest entry in the TZDB is 1844, so this database covers all
      entries for all time periods.
    * Exported identifiers:
        * `kAtcAllZoneContext`
        * `kAtcAllZoneXxx` (e.g. `kAtcAllZoneAmerica_Los_Angeles`)
        * `kAtcAllZoneIdXxx` (e.g. `kAtcAllZoneIdAmerica_Los_Angeles`)

* [src/zonedbtesting](src/zonedbtesting)
    * A limited (10-20) number of zones, from the year 2000 to 10000.
    * Used by unit tests for stability.
    * Exported identifiers:
        * `kAtcTestingZoneContext`
        * `kAtcTestingZoneXxx` (e.g. `kAtcTestingZoneAmerica_Los_Angeles`)
        * `kAtcTestingZoneIdXxx` (e.g. `kAtcTestingZoneIdAmerica_Los_Angeles`)

These files are programmatically generated by scripts in the
[AceTimeTools](https://github.com/bxparks/AceTimeTools) project.

The full list of Zones (and Links) supported by this library is given in the
respective `zone_infos.h` file, which is automatically included by the
`<acetimec.h>` header file.

The IANA TZ database is often updated to track changes to the DST rules in
different countries and regions. The version of the TZ database that was used to
generate the acetimec Zone database is given by:

```C
extern const char kAtcTzDatabaseVersion[];
extern const char kAtcAllTzDatabaseVersion[];
extern const char kAtcTestingTzDatabaseVersion[];
```

For example, these will all be `"2023c"` for the 2023c version of the TZ
database.

The Zone Registry is defined in the respective `zone_registry.h` file which
contains the list of all Zones (and Links) supported by this library. It allows
us to locate the `AtcZoneInfo` pointer using the human readable zone name (e.g.
`"America/Los_Angeles"`) or its 32-bit zone identifier (e.g. `0xb7f7e8f2`).

Each zonedb provides 2 zone registries:

```C
#define kAtcZoneRegistrySize 356
extern const AtcZoneInfo * const kAtcZoneRegistry[356];
#define kAtcZoneAndLinkRegistrySize 595
extern const AtcZoneInfo * const kAtcZoneAndLinkRegistry[595];

#define kAtcAllZoneRegistrySize 356
extern const AtcZoneInfo * const kAtcAllZoneRegistry[356];
#define kAtcAllZoneAndLinkRegistrySize 595
extern const AtcZoneInfo * const kAtcAllZoneAndLinkRegistry[595];

#define kAtcTestingZoneRegistrySize 16
extern const AtcZoneInfo * const kAtcTestingZoneRegistry[16]
#define kAtcTestingZoneAndLinkRegistrySize 17
extern const AtcZoneInfo * const kAtcTestingZoneAndLinkRegistry[17];
```

The `kAtc*ZoneRegistry` and `kAtc*ZoneAndLinkRegistry` are used by the [Zone
Registrar functions](#AtcZoneRegistrar) described below.

<a name="AtcZonedExtra"></a>
### AtcZonedExtra

The `AtcZonedExtra` structure in [zoned_extra.h](src/acetimec/zoned_extra.h)
holds additional meta information about a particular time zone, usually at a
particular epoch seconds:

```C
enum {
  kAtcZonedExtraNotFound = 0,
  kAtcZonedExtraExact = 1,
  kAtcZonedExtraGap = 2,
  kAtcZonedExtraOverlap = 3,
};

typedef struct AtcZonedExtra {
  int8_t type;
  int32_t std_offset_seconds; // STD offset
  int32_t dst_offset_seconds; // DST offset
  int32_t req_std_offset_seconds; // request STD offset
  int32_t req_dst_offset_seconds; // request DST offset
  char abbrev[kAtcAbbrevSize];
} AtcZonedExtra;
```

For `type` of `kAtcZonedExtraExact` and `kAtcZonedExtraOverlap`, the `req_std_offset_seconds` and `req_dst_offset_seconds` will be identical
to the corresponding `std_offset_seconds` and `dst_offset_seconds` parameters.

For `type` `kAtcZonedExtraGap`, which can be returned only by the
`atc_zoned_extra_from_local_date_time()` function below, the `fold` parameter
selects one of the 2 matching `AtcZonedDateTime` following the algorithms
described by Python [PEP 495](https://www.python.org/dev/peps/pep-0495/). Since
the `AtcLocalDateTime` instants before and after normalization are different, we
need 2 different sets of offset seconds:

* `req_std_offset_seconds` and `req_dst_offset_seconds` fields correspond
  to the `AtcLocalDateTime` *before* normalization, and
* `std_offset_seconds` and `dst_offset_seconds` fields correspond to the
  `AtcLocalDateTime` *after* normalization.

There following functions operate on this data structure (analogous to the
functions that work with the `AtcZonedDateTime` data structure):

```C
void atc_zoned_extra_set_error(AtcZonedExtra *extra);

bool atc_zoned_extra_is_error(const AtcZonedExtra *extra);

void atc_zoned_extra_from_epoch_seconds(
    AtcZonedExtra *extra,
    atc_time_t epoch_seconds,
    AtcTimeZone tz);

void atc_zoned_extra_from_unix_seconds(
    AtcZonedExtra *extra,
    int64_t unix_seconds,
    AtcTimeZone tz);

void atc_zoned_extra_from_local_date_time(
    AtcZonedExtra *extra,
    AtcLocalDateTime *ldt,
    AtcTimeZone tz);
```

On error, the `extra.type` field is set to `kAtcZonedExtraNotFound` and
`atc_zoned_extra_is_error()` returns `true`.

<a name="AtcZoneRegistrar"></a>
## AtcZoneRegistrar

The functions in [zone_registrar.h](src/acetimec/zone_registrar.h) allow
searching of the [zone registries](#ZoneDatabaseAndRegistry) by human readable
name (e.g. "America/Los_Angeles") or by a 32-bit numerical zoneId.

The zone identifier is a unique and stable 32-bit integer associated with each
time zone. It was defined in the AceTime library. See the section
[CreateForZoneId](https://github.com/bxparks/AceTime/blob/develop/USER_GUIDE.md#CreateForZoneId)
in the `USER_GUIDE.md` for the AceTime library. A 32-bit integer is often more
convenient in an embedded environment than the human-readable zone name because
the integer is a fixed size and can be stored and retrieved quickly. Examples
are shown below.

To use these registries, first we create and initialize an `AtcZoneRegistrar`
data structure using the `atc_registrar_init()` function:

```C
typedef struct AtcZoneRegistrar {
  const AtcZoneInfo * const * registry;
  uint16_t size;
  bool is_sorted;
} AtcZoneRegistrar;

void atc_registrar_init(
    AtcZoneRegistrar *registrar,
    const AtcZoneInfo * const * registry,
    uint16_t size);
```

Then we can query the registry using either the zoneId or its zone name:

```C
const AtcZoneInfo *atc_registrar_find_by_name(
    const AtcZoneRegistrar *registrar,
    const char *name);

const AtcZoneInfo *atc_registrar_find_by_id(
    const AtcZoneRegistrar *registrar,
    uint32_t zone_id);
```

Here is a sample code to retrieve the `AtcZoneInfo` pointer from the human
readable zone name (e.g. `"America/Los_Angeles"`):

```C
#include <acetimec.h>

AtcZoneRegistrar registrar;

// Perform this only once
void setup()
{
  atc_registrar_init(
      &registrar,
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize);
  ...
}

void retrieve_zone_info_by_name()
{
  const char *name = "America/Los_Angeles";
  const AtcZoneInfo *zone_info = atc_registrar_find_by_name(&registrar, name);
  if (zone_info == NULL) { /*error*/ }
  ...
}
```

Instead of using the string `"America/Los_Angeles"`, we can search for this
timezone by its 32-bit zoneId which is provided by the Zone Database as
`kAtcZoneIdAmerica_Los_Angeles=0xb7f7e8f2`:

```C
#include <acetimec.h>

AtcZoneRegistrar registrar;

// Perform setup() as above

void retrieve_zone_info_by_id()
{
  uint32_t zone_id = kAtcZoneIdAmerica_Los_Angeles;
  const AtcZoneInfo *zone_info = atc_registrar_find_by_id(&registrar, zone_id);
  if (zone_info == NULL) { /*error*/ }
  ...
}
```

The `atc_registrar_init()` function performs an optimization. It evaluates
whether or not the registry is sorted by zone id. If it is, then the search
functions (both `find_by_name()` and `find_by_id()`) will use binary search
algorithm. If the registry is not sorted, then the search functions performs a
linear search through the registry. The binary search algorithm is `O(log(N))`
and the linear search of `O(N)`. The binary search will be far faster than the
linear search if the registry contains more than about 5-10 entries.

The downstream application does not need to use the default zone registries
(`kAtcZoneRegistry` or `kAtcZoneAndLinkRegistry`). It can create its own custom
zone registry, and pass this custom registry into the
`atc_registrar_find_by_name()` or `atc_registrar_find_by_id()` functions.

<a name="Bugs"></a>
## Bugs And Limitations

* No support for constants in flash memory
    * Some microcontrollers (e.g. AVR, ESP8266) use a modified-Harvard memory
      architecture where the program and data are in 2 different address spaces.
    * To place large data constants (e.g. arrays) into program flash memory
      instead of static memory, special (non-standard) compiler directives need
      to be used.
    * On `avr-gcc` compiler, this is `PROGMEM` directive.
    * acetimec library does not use the `PROGMEM` directive, so the library will
      probably will not fit inside an AVR processor.
    * On other microcontrollers (e.g. ARM), constants are automatically placed
      into flash memory and referenced directly from there. No special compiler
      directives are required.
* The `atc_time_t` type is a 32-bit signed integer.
    * This has a range of approximately +/- 68 years around the (adjustable)
      current epoch year.
    * A 64-bit type would solve the range problem, but would consume more flash
      and ram resources. This option may be implemented in the future.

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments, or feature requests for this library,
please use the [GitHub
Discussions](https://github.com/bxparks/acetimec/discussions) for this project.
If you have bug reports, please file a ticket in [GitHub
Issues](https://github.com/bxparks/acetimec/issues). Feature requests should go
into Discussions first because they often have alternative solutions which are
useful to remain visible, instead of disappearing from the default view of the
Issue tracker after the ticket is closed.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
