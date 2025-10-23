# AceTime for C

[![ACUnit Tests](https://github.com/bxparks/acetimec/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/bxparks/acetimec/actions/workflows/unit_tests.yml)

The acetimec library is a date and time zone library for the C language, based
on algorithms and techniques from the
[AceTime](https://github.com/bxparks/AceTime) library for the Arduino
environment. Just like AceTime, this library supports all time zones defined by
the [IANA TZ database](https://www.iana.org/time-zones).

The library is intended to be used in bare-metal (no operating system),
resource-constrained, 32-bit embedded environments. To support all zones in the
TZDB database (about 340 in TZDB 2025b), the processor needs about 64 kiB of
readonly memory and about 8 kiB of RAM. Custom zone registries can be created
with only a subset of timezones to reduce the memory requirements even further.

In its current form, the library is not recommended for 8-bit embedded
environments with less than about 32 kiB memory. Support for 8-bit
microcontrollers may be improved in the future.

The library works in 64-bit environments with megabytes or gigabytes of memory,
but it is not intended for those environments. Those environments likely run
with a full operating system, and there are other date-time and timezone
libraries with full set of features which are more suitable.

The library uses 32-bit integer seconds for internal calculations in order to
reduce resource consumption. That restricts the operational range of the library
to about +/- 68 years of the epoch year. Fortunately, the epoch year is not the
Unix epoch of 1970, but is set to 2050 by default. This allows the upper limit
of this library to be the year 2118. In addition, the epoch year is configurable
at *run-time* to any year from about 0001 to 9999.

The acetimec library does not perform any dynamic allocation of memory
internally. Everything it needs is allocated statically or provided by the
calling program. Applications can choose to allocate the necessary resources
statically, or allocate them on the heap using `malloc()` at startup time and
then never call `free()`.

The functionality provided by acetimec is a subset of the AceTime library,
mostly because the C language does not provide the same level of abstraction and
encapsulation as the C++ language. If the equivalent functionality of AceTime
was attempted in this library, the public API would become too large and
complex, with diminishing returns from the increased complexity. Specifically,
this library implements the only algorithms provided by the
`ExtendedZoneProcessor` class of the AceTime library. It does not implement the
functionality provided by the `BasicZoneProcessor` of the AceTime library.

**Status**: Beta-level, API mostly stable \
**Version**: 0.14.0 (2025-10-21, TZDB 2025b) \
**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Table of Contents

- [Examples](#examples)
- [Installation](#installation)
- [Usage](#usage)
    - [Header File](#header-file)
    - [Constants](#constants)
    - [atc_time_t](#atc_time_t)
    - [Epoch](#epoch)
    - [PlainDate](#plaindate)
    - [PlainTime](#plaintime)
    - [AtcPlainDateTime](#atcplaindatetime)
    - [AtcOffsetDateTime](#atcoffsetdatetime)
    - [AtcZonedDateTime](#atczoneddatetime)
    - [AtcTimeZone](#atctimezone)
    - [AtcZoneProcessor](#atczoneprocessor)
    - [AtcZoneInfo](#atczoneinfo)
    - [Zone Database and Registry](#zone-database-and-registry)
    - [AtcZonedExtra](#atczonedextra)
    - [AtcZoneRegistrar](#atczoneregistrar)
    - [Custom Registry](#custom-registry)
- [Validation](#validation)
- [Bugs And Limitations](#bugs-and-limitations)
- [License](#license)
- [Feedback and Support](#feedback-and-support)
- [Authors](#authors)

## Examples

Here is a basic example from
[examples/hello_acetime.c](examples/hello_acetimec/), which creates an
`AtcZonedDateTime` in the "America/Los_Angeles" time zone, then printing its
epoch seconds and Unix seconds, then converting the time to "America/New_York"
time zone:

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
  printf("==== ZonedDateTime from epoch seconds\n");

  atc_time_t seconds = 3432423;
  printf("Epoch seconds: %ld\n", (long) seconds);

  // Convert epoch seconds to date/time components for given time zone.
  AtcTimeZone tzla = {&kAtcZoneAmerica_Los_Angeles, &processor_la};
  AtcZonedDateTime zdtla;
  atc_zoned_date_time_from_epoch_seconds(&zdtla, seconds, &tzla);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Allocate string buffer for human readable strings.
  char buf[80];
  struct AtcStringBuffer sb;
  atc_buf_init(&sb, buf, 80);

  // Print the date for Los Angeles.
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Print the epoch seconds.
  atc_time_t epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  if (seconds != epoch_seconds) { /*error*/ }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  int64_t unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("==== ZonedDateTime from PlainDateTime with DisambiguateCompatible\n");

  // Start with a PlainDateTime in an overlap.
  AtcPlainDateTime pdt = {2022, 11, 6, 1, 30, 0};
  atc_buf_reset(&sb);
  atc_plain_date_time_print(&sb, &pdt);
  atc_buf_close(&sb);
  printf("PlainDateTime: %s\n", sb.p);

  // Convert components to an AtcZonedDateTime. 2022-11-06 01:30 occurred twice.
  // It is probably most common to want the earlier one, which can be done
  // using either kAtcDisambiguateCompatible or kAtcDisambiguateEarlier.
  atc_zoned_date_time_from_plain_date_time(
      &zdtla, &pdt, &tzla, kAtcDisambiguateCompatible);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, &zdtla);
  atc_buf_close(&sb);
  printf("Los Angeles: %s\n", sb.p);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  printf("Epoch seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);

  printf("======== ZonedDateTime to different time zone\n");

  // convert America/Los_Angeles to America/New_York
  AtcTimeZone tzny = {&kAtcZoneAmerica_New_York, &processor_ny};
  AtcZonedDateTime zdtny;
  atc_zoned_date_time_convert(&zdtla, tzny, &zdtny);
  if (atc_zoned_date_time_is_error(&zdtla)) { /*error*/ }

  // Print the date time.
  atc_buf_reset(&sb);
  atc_zoned_date_time_print(&sb, zdtny);
  atc_buf_close(&sb);
  printf("New York: %s\n", sb.p);

  // Print the epoch seconds.
  epoch_seconds = atc_zoned_date_time_to_epoch_seconds(&zdtla);
  if (epoch_seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  printf("Epoch Seconds: %ld\n", (long) epoch_seconds);

  // Print the unix seconds.
  unix_seconds = atc_zoned_date_time_to_unix_seconds(&zdtla);
  if (unix_seconds == kAtcInvalidUnixSeconds) { /*error*/ }
  printf("Unix seconds: %lld\n", (long long) unix_seconds);
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
  setup();
  print_dates();
}
```
Running this produces the following on the screen:

```
==== ZonedDateTime from epoch seconds
Epoch seconds: 3432423
Los Angeles: 2050-02-09T09:27:03-08:00[America/Los_Angeles]
Epoch seconds: 3432423
Unix seconds: 2528040423
==== ZonedDateTime from PlainDateTime with DisambiguateCompatible
PlainDateTime: 2022-11-06T01:30:00
Los Angeles: 2022-11-06T01:30:00-07:00[America/Los_Angeles]
Epoch seconds: -856884600
Unix seconds: 1667723400
==== Convert ZonedDateTime to different time zone
New York: 2022-11-06T03:30:00-05:00[America/New_York]
Epoch seconds: -856884600
Unix seconds: 1667723400
```

More examples are available at:

- [examples/hello_acetimec_more](examples/hello_acetimec_more/)
    - A longer more detailed example.
- [examples/hello_registrar](examples/hello_registrar/)
    - How to use an `AtcZoneRegistrar` to query zones by name or by zoneId.
- [examples/hello_zonedextra](examples/hello_zonedextra/)
    - Using `ZonedExtra` to retrieve timezone abbreviations.
- [examples/hello_custom_registry](examples/hello_custom_registry/)
    - How to create custom zone registries.

## Installation

I am not familiar with any C language package managers. To obtain this library,
you should manually clone the project into an appropriate place on your
computer:

```
$ git clone https://github.com/bxparks/acetimec
```

There are many different ways that a C library can be incorporated into an
existing project, depending the platform. I do almost all my development on a
Linux box, so I provide the following infrastructure:

- The `./src/Makefile` provides a target `$ make acetimec.a` which compiles the
  `*.c` files to `*.o` files, then collects these object files into a static
  library named `./src/acetimec.a`.
    - The Makefile can generate another version of the archive file called
      `./src/acetimecm.a`. This is an experimental variant which can be ignored
      for now.
- Downstream applications can link to this static library. This can be done by
  passing the `./src/acetimec.a` file to the linker.
- Downstream application source code needs to include the `./src/acetimec.h`
  header file. This can be done using the `-I` flag to the compiler.

If you want to run the unit tests in `./tests`, you need to clone the
[ACUnit](https://github.com/bxparks/acetimec) library as a *sibling* to the
`acetimec` directory:

```
$ git clone https://github.com/bxparks/acunit
```

This is a headers-only library that provides the framework for the unit tests
under the `./tests` directory.

## Usage

### Header File

The header file must be included like this:

```C
#include <acetimec.h>
```

Naming conventions:

- `#define` macros a prefixed by `ACE_TIME_C` (e.g. `ACE_TIME_C_VERSION`)
- C-language constants begin with the prefix `kAtc` (e.g.
  `kAtcInvalidEpochSeconds`)
- struct names and typedefs begin with the prefix `Atc` (e.g.
  `AtcZonedDateTime`)
- zone database are in directories named:
    - `zonedb2000`
    - `zonedb2025`
    - `zonedball`
- the corresponding objects and symbols in those files are prefixed as:
    - `kAtcZonedb2000` (e.g. `kAtcZonedb2000ZoneAmerica_Los_Angeles`)
    - `kAtcZonedb2025` (e.g. `kAtcZonedb2025ZoneAmerica_Los_Angeles`)
    - `kAtcZonedball` (e.g. `kAtcZonedballZoneAmerica_Los_Angeles`)

### Constants

A number of public constants are provided by this library:

**range limits**

- `kAtcInvalidYear - INT16_MIN`: (-32736), an invalid year
- `kAtcInvalidEpochDays = INT32_MIN`: (-2147483648), an invalid epoch days
- `kAtcInvalidEpochSeconds = INT32_MIN`: (-2147483648), an invalid epoch seconds
- `kAtcInvalidUnixDays = INT32_MIN`: (-2147483648), an invalid unix days
- `kAtcInvalidUnixSeconds = INT64_MIN`: (-9,223,372,036,854,775,808), an invalid
  unix seconds
- `kAtcAbbrevSize = 8`: time zone abbreviation size including trailing NUL

**ISO Weekdays**
- `kAtcIsoWeekdayMonday = 1`
- `kAtcIsoWeekdayTuesday = 2`
- `kAtcIsoWeekdayWednesday = 3`
- `kAtcIsoWeekdayThursday = 4`
- `kAtcIsoWeekdayFriday = 5`
- `kAtcIsoWeekdaySaturday = 6`
- `kAtcIsoWeekdaySunday = 7`

**disambiguation directive if overlap or gap**

- `kAtcDisambiguateCompatible = 0`: earlier for overlap, later for gap
- `kAtcDisambiguateEarlier = 1`: always pick earlier
- `kAtcDisambiguateLater = 2`: always pick later
- `kAtcDisambiguateReversed = 3`: opposite of Compatible

**resolution of overlap or gap**

- `kAtcResolvedUnique = 0`: resolved to unique AtcZonedDateTime
- `kAtcResolvedOverlapEarlier = 1`: in overlap and resolved to earlier time
- `kAtcResolvedOverlapLater = 2`: in overlap and resolved to later time.
- `kAtcResolvedGapEarlier = 3`: in gap and resolved to earlier time
- `kAtcResolvedGapLater = 4`: in gap and resolved to later time

**AtcZonedExtra fold_type**

- `kAtcFoldTypeNotFound = 0`: AtcZonedExtra was not found (should never happen)
- `kAtcFoldTypeExact = 1`: the PlainDateTime or epoch seconds was unique
- `kAtcFoldTypeOverlap = 2`: the PlainDateTime was in an overlap
- `kAtcFoldTypeGap = 3`: the PlainDateTime was in a gap

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

A handful of functions return the unix seconds as an `int64_t` type. A 64-bit
integer is required because a 32-bit integer would overflow in the year 2038 due
to the [Year 2038 Problem](https://en.wikipedia.org/wiki/Year_2038_problem).

### Epoch

The functions in [epoch.h](src/acetimec/epoch.h) provide features related to
the epoch used by the acetimec library. By default, the epoch is 2050-01-01
00:00:00 UTC, which allows the 32-bit `ace_time_t` type to support dates from
the year 2000 until the year 2100, at a minimum. However, unlike most timezone
libraries, the epoch year can be changed at runtime so that the `ace_time_t` can
be used to support any dates within approximately +/- 50 years of the epoch
year.

The following functions are used to get and set the epoch year:

- `int16_t atc_get_current_epoch_year(void)`
    - Returns the current epoch year.
- `void atc_set_current_epoch_year(int16_t year)`
    - Sets the current epoch year to `year`.

**Warning**: If the epoch year is changed using the
`atc_set_current_epoch_year()` function, then the `atc_processor_init()`
function (see [AtcZoneProcessor](#AtcZoneProcessor)) must be called to
reinitialize any instance of `AtcZoneProcessor` that may have used a different
epoch year.

The following convenience functions return the range of validity of the
`ace_time_t` type:

- `int16_t atc_epoch_valid_year_lower(void)`
    - Returns the lower bound of the year that can be represented by
      `ace_time_t`.
    - This currently returns `epoch_year - 50` as a conservative estimate.
    - The actual lower bound is 10-15 years higher, and a future version of the
      library may update the value returned by this function.
- `int16_t atc_epoch_valid_year_upper(void)`
    - Returns the upper bound of the year that can be represented by
      `ace_time_t`.
    - This currently returns `epoch_year + 50` as a conservative estimate.
    - The actual upper bound is 10-15 years higher, and a future version of the
      library may update the value returned by this function.

The following are low level internal functions that convert a given `(year,
month, day)` triple in the proleptic Gregorian calendar to the number of days
from an arbitrary, but fixed, internal epoch date (currently the year 2000).
They are used as the basis for converting the Gregorian date to the number of
offset days from the user-adjustable epoch year. They are not expected to be
used by client applications.

- `int32_t atc_convert_to_days(int16_t year, uint8_t month, uint8_t day)`
- `void atc_convert_from_days(int16_t days, uint8_t *year, uint8_t *month,
   uint8_t *day)`

### PlainDate

The functions in [plain_date.h](src/acetimec/plain_date.h) provide features
related to the Gregorian `(year, month, day)` triple, call a "PlainDate". Many
date-time libraries provide an object representation of PlainDate. In this
library, the data struct would have been called `AtcPlainDate`. However, it
seemed too cumbersome in the C-language to create this extra layer of
abstraction. Therefore, this header file contains utility functions which accept
or update the (year, month, day) parameters separately.

These functions do not know about the time components (hour, minute, second) or
the timezone. The PlainDate components represent either the local date, the UTC
date, or an abstract Gregorian calendar date, depending on context.

The `atc_is_leap_year()` functions returns true if the given year is a leap
year:

```C
bool atc_is_leap_year(int16_t year);
```

The `atc_plain_date_is_valid()` function returns true if the PlainDate triple
`(year, month, day)` is a valid Gregorian calendar date within the year range
[1, 9999]. Otherwise, it returns false.

```C
bool atc_plain_date_is_valid(int16_t year, uint8_t month, uint8_t day);
```

The `atc_plain_date_day_of_week()` function returns the ISO day of week of the
given `(year, month, day)` date. The PlainDate triple is *not* validated. The
ISO weekday starts with Monday as 1, and ending with Sunday as 7:

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

uint8_t atc_plain_date_day_of_week(int16_t year, uint8_t month, uint8_t day)`
```

The following 2 functions convert the Gregorian date to and from the number of
days from the current epoch (given by `atc_get_current_epoch_year()`). These
functions should return the correct value for any year in the range of `0 < year
< 10000`.

The `atc_plain_date_to_epoch_days()` function validates the PlainDate triple and
returns `kAtcInvalidEpochDays` if invalid. However, the
`atc_plain_date_from_epoch_days()` does *not* validate the `epoch_days` input
argument.

```C
int32_t atc_plain_date_to_epoch_days(
    int16_t year, uint8_t month, uint8_t day);

void atc_plain_date_from_epoch_days(
    int32_t epoch_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day);
```

The next two functions named `unix_days` are exactly the same as the
`epoch_days` versions but they use the Unix epoch of 1970-01-01. If the given
values are invalid, then `atc_plain_date_to_unix_days()` returns
`kAtcInvalidUnixDays`. On the other hand, the `atc_plain_date_from_unix_days()`
function does not validate the `unix_days`, because it is awkward to return an
error code back to the user.

```C
int32_t atc_plain_date_to_unix_days(
    int16_t year, uint8_t month, uint8_t day);

void atc_plain_date_from_unix_days(
    int32_t unix_days,
    int16_t *year,
    uint8_t *month,
    uint8_t *day);
```

The following functions increment and decrement the date by one day. The
PlainDate triple (year, month, day) arguments are *not* validated.

```C
void atc_plain_date_increment_one_day(
    int16_t *year, uint8_t *month, uint8_t *day);

void atc_plain_date_decrement_one_day(
    int16_t *year, uint8_t *month, uint8_t *day);
```

### PlainTime

The [plain_time.h](src/acetimec/plain_time.h) file contains functions related to
the (hour, minute, second) parameters without any reference to a date or a
timezone. If we followed other date-time libraries, this library would provide
an `AtcPlainTime` struct to represent this object. However, it seemed too
cumbersome to create that extra layer of abstraction in the C-language. So this
header file contains functions which consume or produce the (hour, minute,
second) fields separately.

The `atc_plain_time_is_valid()` validates the PlainTime triple, and returns true
if the parameters are valid (within the interval [00:00:00, 23:59:59]):

```C
bool atc_plain_time_is_valid(uint8_t hour, uint8_t minute, uint8_t second);
```

The `atc_plain_time_to_seconds()` function converts the PlainTime triple to the
number of seconds since midnight 00:00:00. If the arguments are invalid,
according to `atc_plain_time_is_valid()`, then this function returns
`kAtcInvalidSeconds`.

```C
int32_t atc_plain_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second);
```

### AtcPlainDateTime

The functions in [plain_date_time.h](src/acetimec/plain_date_time.h) operate
on the `AtcPlainDateTime` type which represents a date-time without
reference to a time zone. This can represent the local date-time, the UTC time,
or an abstract Gregorian date-time, depending on context.

```C
typedef struct AtcPlainDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} AtcPlainDateTime;
```

Below are the functions which operate on this data type.

The `atc_plain_date_time_set_error(pdt)` marks the given `pdt` as invalid. This
causes `atc_plain_date_time_is_error(pdt)` to return `true`.

```C
void atc_plain_date_time_set_error(AtcPlainDateTime *pdt);

bool atc_plain_date_time_is_error(const AtcPlainDateTime *pdt);
```

The `atc_plain_date_time_is_valid()` function validates the `AtcPlainDateTime`
object by calling `atc_plain_date_is_valid()` and `atc_plain_time_is_valid()`,
and returns `true` if both parts are valid:

```C
bool atc_plain_date_time_is_valid(const AtcPlainDateTime *pdt);
```

The `atc_plain_date_time_to_epoch_seconds()` function converts the given
`AtcPlainDateTime` into its `atc_time_t` epoch seconds. If `pdt` is not valid or
an error occurs, the function returns `kAtcInvalidEpochSeconds`.

The `atc_plain_date_time_from_epoch_seconds()` function converts the given epoch
seconds into the `AtcPlainDateTime` components. If `pdt` is not valid or an
error occurs, the `pdt` is set to its error value.

```C
atc_time_t atc_plain_date_time_to_epoch_seconds(
    const AtcPlainDateTime *pdt);

void atc_plain_date_time_from_epoch_seconds(
    AtcPlainDateTime *pdt,
    atc_time_t epoch_seconds);
```

The next two functions with the names `unix_seconds` are the same the previous
functions named `epoch_seconds`, except that they operate on `unix_seconds`
which uses the Unix epoch 1970-01-01. The `unix_seconds` is a type `int64_t`
because a 32-bit integer would overflow in the year 2038.

```C
int64_t atc_plain_date_time_to_unix_seconds(
    const AtcPlainDateTime *pdt);

void atc_plain_date_time_from_unix_seconds(
    AtcPlainDateTime *pdt,
    int64_t unix_seconds);
```

The `atc_plain_date_time_print()` function converts the given `pdt` into a
string in RFC 3339/ISO 8601 formatted into the string buffer `sb`:

```C
void atc_plain_date_time_print(
    AtcStringBuffer *sb,
    const AtcPlainDateTime *pdt);
```

### AtcOffsetDateTime

The functions in [offset_date_time.h](src/acetimec/offset_date_time.h) operate
on the `AtcOffsetDateTime` type which represents a date-time with a fixed offset
from UTC. This object is required for implementation of the `AtcZonedDateTime`
functions, and is intended mostly for internal use. It is documented here for
completeness.

```C
typedef struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t resolved;

  int32_t offset_seconds;
} AtcOffsetDateTime;
```

The memory layout of `AtcOffsetDateTime` was designed to be identical to
`AtcPlainDateTime` so that functions that accept a pointer to `AtcPlainDateTime`
can be given a pointer to `AtcOffsetDateTime` as well.

Here are the functions that operate on the `AtcOffsetDateTime` object. They
should look familiar because they follow the same pattern as the ones for the
`AtcPlainDateTime` object.

The `atc_offset_date_time_set_error(odt)` marks the given `odt` as invalid .
This causes `atc_offset_date_time_is_error(odt)` to return `true`.

```C
void atc_offset_date_time_set_error(AtcOffsetDateTime *odt);

bool atc_offset_date_time_is_error(const AtcOffsetDateTime *odt);
```

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`AtcOffsetDateTime` into its `atc_time_t` epoch seconds, taking into account the
`offset_seconds` field. If an error occurs, the function returns
`kAtcInvalidEpochSeconds`.

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`epoch_seconds` and `offset_seconds` into the `AtcOffsetDateTime` components.

```C
atc_time_t atc_offset_date_time_to_epoch_seconds(
    const AtcOffsetDateTime *odt);

void atc_offset_date_time_from_epoch_seconds(
    AtcOffsetDateTime *odt,
    atc_time_t epoch_seconds,
    int32_t offset_seconds);
```

The next two functions are the Unix seconds versions of the above:

```C
int64_t atc_offset_date_time_to_unix_seconds(
    const AtcOffsetDateTime *odt);

void atc_offset_date_time_from_unix_seconds(
    AtcOffsetDateTime *odt,
    int64_t unix_seconds,
    int32_t offset_seconds);
```

The `atc_offset_date_time_print()` function converts a `AtcOffsetDateTime` into
human readable form:

```C
void atc_offset_date_time_print(
    AtcStringBuffer *sb,
    const AtcOffsetDateTime *odt);
```

### AtcZonedDateTime

The functions in [zoned_date_time.h](src/acetimec/zoned_date_time.h) operate
on the `AtcZonedDateTime` data structure, which is identical to the
`AtcOffsetDateTime` data structure with the addition of a reference to the TZDB
time zone through the `AtcTimeZone` object. (The `AtcTimeZone` object is
explained in the next section).

```C
typedef struct AtcZonedDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t resolved;

  int32_t offset_seconds;
  AtcTimeZone tz;
```

The memory layout of `AtcZonedDateTime` was designed to be identical to
`AtcOffsetDateTime`, so that functions that accept a pointer to
`AtcOffsetDateTime` can also accept pointers to `AtcZonedDateTime`.

The following functions should look familiar:

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

void atc_zoned_date_time_print(
    AtcStringBuffer *sb,
    const AtcZonedDateTime *zdt);
```

Briefly, these functions are:

- `atc_zoned_date_time_set_error(zdt)`
    - marks the given `zdt` as an error
- `atc_zoned_date_time_is_error(zdt)`
    - checks if `zdt` is in an error state
- `atc_zoned_date_time_from_epoch_seconds()`
    - Converts the given `epoch_seconds` and `tz` into the `AtcZonedDateTime`
      components.
    - If an error occurs, the `zdt` object will be set to an error state which
      can be queried using `atc_zoned_date_time_is_error()`.
- `atc_zoned_date_time_to_epoch_seconds()`
    - Converts the given `AtcZonedDateTime` into its `atc_time_t` epoch seconds,
      taking into account the time zone defined by the `tz` field inside the
      `AtcZonedDatetime`.
    - If an error occurs, the function returns `kAtcInvalidEpochSeconds`.
- `atc_zoned_date_time_from_unix_seconds()`
    - Converts the given `unix_seconds` and `tz` into the `AtcZonedDateTime`
      components.
    - If an error occurs, the `zdt` object will be set to an error state which
      can be queried using `atc_zoned_date_time_is_error()`.
- `atc_zoned_date_time_to_unix_seconds()`
    - Converts the given `AtcZonedDateTime` into its `atc_time_t` epoch seconds,
      taking into account the time zone defined by the `tz` field inside the
      `AtcZonedDatetime`.
    - If an error occurs, the function returns `kAtcInvalidUnixSeconds`.

The `atc_zoned_date_time_from_plain_date_time()` function converts the local
wall clock defined by `AtcPlainDateTime` to the `AtcZonedDateTime`, taking into
account the time zone defined by `tz`.

```C
void atc_zoned_date_time_from_plain_date_time(
    AtcZonedDateTime *zdt,
    const AtcPlainDateTime *pdt,
    AtcTimeZone *tz,
    uint8_t disambiguate);
```

The `disambiguate` argument determines how a local time in a gap or overlap will
be handled. The resulting `zdt.resolved` field is available to indicate how the
ambiguity (if any) was resolved:

- `kAtcResolvedUnique`: AtcPlainDateTime was unique
- `kAtcResolvedOverlapEarlier`: AtcPlainDateTime matched an overlap and was
    resolved to the earlier datetime
- `kAtcResolvedOverlapLater`: AtcPlainDateTime matched an overlap and was
    resolved to the later datetime
- `kAtcResolvedGapEarlier`: AtcPlainDateTime matched a gap and was resolved to
    the earlier datetime
- `kAtcResolvedGapLater`: AtcPlainDateTime matched a gap and was resolved to the
    later datetime

The `disambiguate` parameter is *not* required for the
`atc_zoned_date_time_from_epoch_seconds()` function or the
`atc_zoned_date_time_from_unix_seconds()` function because the conversion from
an `epoch_seconds` or `unix_seconds` to an `AtcZonedDateTime` can never produce
a gap or overlap.

The parameter is inspired by the `disambiguation` parameter in the
[Temporal](https://tc39.es/proposal-temporal/docs/zoneddatetime.html) Javascript
library, and the `disambiguate` parameter in the
[Whenever](https://whenever.readthedocs.io/en/latest/overview.html#ambiguity-in-timezones)
Python library.

The `atc_zoned_date_time_convert()` function converts an `AtcZonedDateTime`
instance from one time zone to another. The `from` instance of
`AtcZonedDateTime` contains the original time zone. The destination time zone is
`to_tz`. Upon conversion, the `to` instance of `AtcZonedDateTime` will contain
the datetime in the new time zone. If the `from` datetime is invalid, or
something goes wrong in the conversion, the `to` instance will be set to an
error state, which can be checked using the `atc_zoned_date_time_is_error()`
function.

```C
void atc_zoned_date_time_convert(
    const AtcZonedDateTime *from,
    AtcTimeZone to_tz,
    AtcZonedDateTime *to);
```

The `atc_zoned_date_time_normalize()` function is required when an instance of
`AtcZonedDateTime` is modified by directly setting one of its fields, causing
the internal state of the object to become inconsistent. The normalization
process creates an internal `AtcPlainDateTime` object from the date and time
fields, and evaluates the corresponding `AtcZonedDateTime` value using the
embedded timezone object. Since the look up is performed using the
AtcPlainDateTime, the `disambiguate` parameter is required for this
normalization function.

```C
void atc_zoned_date_time_normalize(
    AtcZonedDateTime *zdt,
    uint8_t disambiguate);
```

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

### AtcZoneProcessor

The `AtcZoneProcessor` data structure provides a workspace for the various
internal functions that perform time zone calculations. The internal details
should be considered to be private and subject to change without notice.

One of this data type should be created statically for each time zone used by
the downstream application. Another possibility is to create one on the heap at
startup time, then never freed. This object is the largest consumer of memory.
The fact `AtcTimeZone` holds onto an `AtcZoneProcessor` by pointer allows the
`acetimec` library to avoid any memory allocation during runtime.

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

**Warning**: The `AtcZoneProcessor` is stateful and not thread-safe.
Thread-safety must be provided externally.

### AtcZoneInfo

The `AtcZoneInfo` data structure in [zone_info.h](src/acetimec/zone_info.h)
defines the DST transition rules of a single time zone. The pointer to the
`AtcZoneInfo` is meant to be passed around as opaque object for the most part
since most of the fields are meant for internal consumption. Normally, the
`AtcZoneInfo` pointer will point to an entry in a larger readonly ZoneDB
database (see next section).

There are 3 accessor functions that retrieve information from inside the ZoneDB
database which may be useful for end-users:

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

### Zone Database and Registry

There are 4 zoneinfo databases provided by this library and available through
the default `<acetimec.h>` header file:

- [src/zonedb2000](src/zonedb2000)
    - All TZDB zones, from the year 2000 to 10000.
    - Exported identifiers:
        - `kAtcZonedb2000ZoneContext`
        - `kAtcZonedb2000ZoneXxx` (e.g. `kAtcZonedb2000ZoneAmerica_Los_Angeles`)
        - `kAtcZonedb2000ZoneIdXxx` (e.g. `kAtcZonedb2000ZoneIdAmerica_Los_Angeles`)
- [src/zonedb2025](src/zonedb2025)
    - All TZDB zones, from the year 2025 to 10000.
    - Exported identifiers:
        - `kAtcZonedb2025ZoneContext`
        - `kAtcZonedb2025ZoneXxx` (e.g. `kAtcZonedb2025ZoneAmerica_Los_Angeles`)
        - `kAtcZonedb2025ZoneIdXxx` (e.g. `kAtcZonedb2025ZoneIdAmerica_Los_Angeles`)
- [src/zonedball](src/zonedball)
    - All TZDB zones, from the year 1800 to 10000.
    - The earliest entry in the TZDB is 1844, so this database covers all
      entries for all time periods.
    - Exported identifiers:
        - `kAtcZonedballZoneContext`
        - `kAtcZonedballZoneXxx` (e.g. `kAtcZonedballZoneAmerica_Los_Angeles`)
        - `kAtcZonedballZoneIdXxx` (e.g. `kAtcZonedballZoneIdAmerica_Los_Angeles`)
- [src/zonedbtesting](src/zonedbtesting)
    - A limited (10-20) number of zones, from the year 2000 to 10000.
    - Used by unit tests for stability.
    - Exported identifiers:
        - `kAtcTestingZoneContext`
        - `kAtcTestingZoneXxx` (e.g. `kAtcTestingZoneAmerica_Los_Angeles`)
        - `kAtcTestingZoneIdXxx` (e.g. `kAtcTestingZoneIdAmerica_Los_Angeles`)

The full list of Zones (and Links) supported by this library is given in the
respective `zone_infos.h` file, which is automatically included by the
`<acetimec.h>` header file.

The IANA TZ database is often updated to track changes to the DST rules in
different countries and regions. The version of the TZ database that was used to
generate the acetimec Zone database is given by:

```C
extern const char kAtcZonedb2000TzDatabaseVersion[];
extern const char kAtcZonedb2025TzDatabaseVersion[];
extern const char kAtcZonedballTzDatabaseVersion[];
extern const char kAtcTestingTzDatabaseVersion[];
```

For example, these will all be `"2025b"` for the 2025b version of the TZ
database.

The Zone Registry is defined in the respective `zone_registry.h` file which
contains the list of all Zones (and Links) supported by this library. It allows
us to locate the `AtcZoneInfo` pointer using the human readable zone name (e.g.
`"America/Los_Angeles"`) or its 32-bit zone identifier (e.g. `0xb7f7e8f2`).

Each zonedb provides 2 zone registries named `kAtc*ZoneRegistry` and
`kAtc*ZoneAndLinkRegistry`:

```C
#define kAtcZonedb2000ZoneRegistrySize 340
extern const AtcZoneInfo * const kAtcZonedb2000ZoneRegistry[340];
#define kAtcZonedb2000ZoneAndLinkRegistrySize 597
extern const AtcZoneInfo * const kAtcZonedb2000ZoneAndLinkRegistry[597];

#define kAtcZonedb2025ZoneRegistrySize 340
extern const AtcZoneInfo * const kAtcZonedb2025ZoneRegistry[340];
#define kAtcZonedb2025ZoneAndLinkRegistrySize 597
extern const AtcZoneInfo * const kAtcZonedb2025ZoneAndLinkRegistry[597];

#define kAtcZonedballZoneRegistrySize 340
extern const AtcZoneInfo * const kAtcZonedballZoneRegistry[340];
#define kAtcZonedballZoneAndLinkRegistrySize 597
extern const AtcZoneInfo * const kAtcZonedballZoneAndLinkRegistry[597];

#define kAtcTestingZoneRegistrySize 16
extern const AtcZoneInfo * const kAtcTestingZoneRegistry[16]
#define kAtcTestingZoneAndLinkRegistrySize 17
extern const AtcZoneInfo * const kAtcTestingZoneAndLinkRegistry[17];
```

The `kAtc*ZoneRegistry` and `kAtc*ZoneAndLinkRegistry` are used by the [Zone
Registrar functions](#AtcZoneRegistrar) described below.

### AtcZonedExtra

The `AtcZonedExtra` structure in [zoned_extra.h](src/acetimec/zoned_extra.h)
is a companion to the `AtcZoneDateTime` object. It is retrieved through query
functions very similar to those used to populate an `AtcZonedDateTime` object.

The `AtcZonedExtra` object holds additional ZonedDateTime meta information for a
particular time zone, at a particular epoch seconds, or at a particular
PlainDateTime. The main reason to consult this object is to retrieve the
timezone abbreviation at a particular date-time (e.g. "PDT" for Pacific Daylight
Time, or "CET" for Central European Time). That information is not contained in
the `AtcZonedDataTime`. The other fields in the `AtcZonedExtra` object are
related to how the epoch second or PlainDateTime was resolved during a gap or an
overlap. But most users will probably be satisfied by the information provided
by the `AtcZonedDateTime.resolved` field.

```C
enum {
  kAtcFoldTypeNotFound = 0,
  kAtcFoldTypeExact = 1,
  kAtcFoldTypeGap = 2,
  kAtcFoldTypeOverlap = 3,
};

typedef struct AtcZonedExtra {
  int8_t fold_type;
  char abbrev[kAtcAbbrevSize];
  int32_t std_offset_seconds; // STD offset
  int32_t dst_offset_seconds; // DST offset
  int32_t req_std_offset_seconds; // request STD offset
  int32_t req_dst_offset_seconds; // request DST offset
} AtcZonedExtra;
```

For `type` of `kAtcFoldTypeExact` and `kAtcFoldTypeOverlap`, the `req_std_offset_seconds` and `req_dst_offset_seconds` will be identical
to the corresponding `std_offset_seconds` and `dst_offset_seconds` parameters.

For `type` `kAtcFoldTypeGap`, which can be returned only by the
`atc_zoned_extra_from_plain_date_time()` function below, the `disambiguate`
parameter extends the invalid time backwards or forwards away from the gap. We
need 2 different sets of offset seconds:

- `req_std_offset_seconds` and `req_dst_offset_seconds` fields correspond
  to the `AtcPlainDateTime` *before- normalization, and
- `std_offset_seconds` and `dst_offset_seconds` fields correspond to the
  `AtcPlainDateTime` *after- normalization.

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

void atc_zoned_extra_from_plain_date_time(
    AtcZonedExtra *extra,
    AtcPlainDateTime *pdt,
    AtcTimeZone tz,
    uint8_t disambiguate);
```

On error, the `extra.fold_type` field is set to `kAtcFoldTypeNotFound` and
`atc_zoned_extra_is_error()` returns `true`.

See [examples/hello_zonedextra](examples/hello_zonedextra) for an example of
creating this object to retrieve the timezone abbreviation and other extra
information.

### AtcZoneRegistrar

The functions in [zone_registrar.h](src/acetimec/zone_registrar.h) allow
searching of timezones in the zone registries [zone
registries](#zone-database-and-registry) by human readable name (e.g.
"America/Los_Angeles") or by a unique 32-bit numerical zoneId.

The zoneId is a unique and stable 32-bit integer associated with each
time zone. It was defined in the AceTime library. See the section
[CreateForZoneId](https://github.com/bxparks/AceTime/blob/develop/USER_GUIDE.md#CreateForZoneId)
in the `USER_GUIDE.md` for the AceTime library. A 32-bit integer is often more
convenient in an embedded environment than the human-readable zone name because
the integer is a fixed size and can be stored and retrieved quickly. Examples
are shown below.

To search these registries, first we create and initialize an `AtcZoneRegistrar`
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

// Perform this only once
void setup()
{
  atc_registrar_init(
      &registrar,
      kAtcZoneAndLinkRegistry,
      kAtcZoneAndLinkRegistrySize);
  ...
}

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
functions (both `find_by_name()` and `find_by_id()`) will use the binary search
algorithm. If the registry is not sorted, then the search functions performs a
linear search through the registry. The binary search algorithm is `O(log(N))`
and the linear search is `O(N)`. The binary search will be far faster than the
linear search if the registry contains more than about 5-10 entries.

The downstream application does not need to use the default zone registries
(`kAtcZoneRegistry` or `kAtcZoneAndLinkRegistry`). It can create its own custom
zone registry, and pass this custom registry into the
`atc_registrar_find_by_name()` or `atc_registrar_find_by_id()` functions.

See [examples/hello_registrar](examples/hello_registrar) for an example of
how to create and initialize a registrar object to query a zone database.

### Custom Registry

The zone databases (`zonedb2000`, `zonedb2025`,  `zonedball`) provide predefined
registries of all the Zones and Links contained in the databases. For example,
the `zonedb2000` database provides the `kAtcZonedb2000ZoneRegistry` and the
`kAtcZonedb2000ZoneAndLinkRegistry` registries. The registry is passed to
`atc_registrar_init()` function to initialize the `AtcZoneRegistrar` object.

One consequence of using a predefined registry is that it pulls all zones (and
links) defined in the particular zone database. For example, the
`kAtcZonedb2000ZoneRegistry` for TZDB 2025b contains 340 zones.

Some application may not have enough memory to support all 340 zones. Those
applications can choose to define a custom registry instead. The registry must
have the same type signature as a predefined registry, like this:

```C
// Initialize the custom registry with 5 zones, from the zonedb2025 database.
const AtcZoneInfo * const kCustomRegistry[]  = {
  &kAtcZonedb2025ZoneAmerica_Los_Angeles, // 0xb7f7e8f2, America/Los_Angeles
  &kAtcZonedb2025ZoneAmerica_Denver, // 0x97d10b2a, America/Denver
  &kAtcZonedb2025ZoneAmerica_Chicago, // 0x4b92b5d4, America/Chicago
  &kAtcZonedb2025ZoneAmerica_New_York, // 0x1e2a7654, America/New_York
  &kAtcZonedb2025ZoneEurope_London, // 0x5c6a84ae, Europe/London
};

const uint16_t kCustomRegistrySize = sizeof(kCustomRegistry) /
    sizeof(const AtcZoneInfo *);
```

We can then initialize the `AtcZoneRegistrar` object as usual, but using the
custom registry:

```C
AtcZoneRegistrar registrar;

void setup()
{
  atc_registrar_init(&registrar, kCustomRegistry, kCustomRegistrySize);
  ...
}
```

See [examples/hello_custom_registry](examples/hello_custom_registry) for
an example of a custom registry.

## Validation

Validation of the `acetimec` library involves validating the algorithms in the
[src/acetimec](src/acetimec) directory and the various zoneinfo
databases over their respective year range of validity:

- [zonedb2000](src/zonedb2000/): from year 2000 until 2200
- [zonedb2025](src/zonedb2025/): from year 2025 until 2200
- [zonedball](src/zonedball/)): from year 1800 until 2200 (the earliest date in
  TZDB is 1844)

For each `zonedb*` database, the DST transitions, epochseconds, and timezone
abbreviations were calculated using `acetimec` over the year range of validity,
and compared against the same information generated from the following
first-party and third-party libraries:

- [C++11/14/17 Hinnant date](https://github.com/HowardHinnant/date) library
- [GNU libc time](https://www.gnu.org/software/libc/libc.html) library
- [C# Noda Time](https://nodatime.org) library
- [Python whenever](https://pypi.org/project/whenever/)
- [AceTime](https://github.com/bxparks/AceTime): Arduino C++ version of AceTime
- [acetimego](https://github.com/bxparks/acetimego): Go or TinyGo version of
  AceTime
- [acetimepy](https://github.com/bxparks/acetimepy): Python version of AceTime

The results from `acetimec` library were identical to the above libraries, which
gives a solid indication that the code and zone databases of `acetimec` are
correct.

The following 3rd party libraries were found to be non-conformant with
`acetimec` for various reasons:

- [Python pytz](https://pypi.org/project/pytz/): pytz cannot handle years after
  2038
- [Python dateutil](https://pypi.org/project/python-dateutil/): dateutil cannot
  handle years after 2038
- [Python 3.9 zoneinfo](https://docs.python.org/3/library/zoneinfo.html): 31
  zones produce incorrect DST offsets
- Java JDK 11
  [java.time](https://docs.oracle.com/en/java/javase/11/docs/api/java.base/java/time/package-summary.html)
  library from year 1800 until 2200
    * 3 IANA timezones are missing from `java.time`
    * ~100 zones seem to produce incorrect DST offsets
    * ~7 zones seem to produce incorrect epochSeconds
- [Go lang `time` package](https://pkg.go.dev/time): 23 zones produce incorrect
  results

## Bugs And Limitations

- No support to store constants in flash memory on some microcontrollers.
    - Some microcontrollers (e.g. AVR, ESP8266) use a modified-Harvard memory
      architecture where the program and data are in 2 different address spaces.
    - To place large data constants (e.g. arrays) into program flash memory
      instead of static memory, special (non-standard) compiler directives need
      to be used.
    - On `avr-gcc` compiler, this is `PROGMEM` directive.
    - The `acetimec` library does not use the `PROGMEM` directive, so the
      library will probably will not fit inside an AVR processor.
    - On other microcontrollers (e.g. ARM), constants are automatically placed
      into flash memory and referenced directly from there. No special compiler
      directives are required.
- All internal computations of DST transitions are performed using 32-bit
  integers representing the number of seconds from the current epoch year.
    - This has a range of approximately +/- 68 years around the (adjustable)
      current epoch year.
    - Even if the `int64_t` unix seconds functions are called, the range of unix
      seconds are limited by the same +/- 68 years.
    - An application can go beyond that interval by manually changing the
      current epoch year using `atc_set_current_epoch_year()`. But this can get
      tricky, and is not recommended.
- Edge case behavior and bugs may exist for some functions.
    - There may be bugs around the year 0001 or the year 9999.
    - There may be bugs at the lower end of the current epoch range
      (currentEpochYear - 68 years) or upper end (currentEpochYear + 68 years).
- Non-optimal support for 8-bit processor environments.
    - The [AceTime](github.com/bxparks/AceTime) library supports a zone database
      format that is optimized for 8-bit microcontrollers with even more severe
      memory limitations.
    - The `acetimec` library does not support those smaller zonedb formats. It
      may be added in the future.
- Thread-safety
    - The library is *not* thread-safe. In particular, the `AtcZoneProcessor`
      contains cached data, and most objects (e.g. `AtcZonedDateTime`) are
      mutable.
    - If the library is used in a multi-threaded environment, thread-safety must
      be provided externally.

## License

[MIT License](https://opensource.org/licenses/MIT)

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

## Authors

- Created by Brian T. Park (brian@xparks.net).
