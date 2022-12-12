# AceTime for C

[![ACUnit Tests](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml)

Time zone library for the C language, based on algorithms and techniques from
the [AceTime](https://github.com/bxparks/AceTime) library for the Arduino
programming environment. Just like AceTime, this library supports all time zones
defined by the [IANA TZ database](https://www.iana.org/time-zones).

The functionality provided by this library (in C) is a subset of the AceTime
library (in C++), mostly because the C language does not provide the same level
of expressiveness,  abstraction, and encapsulation of the C++ language. If the
equivalent functionality of AceTime was attempted in this library, the public
API would become too large and complex, with diminishing returns from the
increased complexity. Therefore, I decided that this library would implement the
only algorithms provided by the `ExtendedZoneProcessor` class of the AceTime
library. It does not implement the functionality provided by the
`BasicZoneProcessor` of the AceTime library.

Due to time constraints, this README document provides only a small fraction of
the documentation provided by the README.md and USER_GUIDE.md documents of the
AceTime library. If you need more detailed information, please consult those
latter documents.

**Version**: 0.5.0 (2022-12-04, TZDB version 2022g)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Table of Contents

* [Example](#Example)
* [Installation](#Installation)
* [Usage](#Usage)
    * [Header File](#HeaderFile)
    * [Constants](#Constants)
    * [atc_time_t](#AtcTimeT)
    * [Epoch](#Epoch)
    * [AtcLocalDate](#AtcLocalDate)
    * [AtcLocalDateTime](#AtcLocalDateTime)
    * [AtcOffsetDateTime](#AtcOffsetDateTime)
    * [AtcZonedDateTime](#AtcZonedDateTime)
    * [AtcZoneProcessing](#AtcZoneProcessing)
    * [AtcZoneInfo](#AtcZoneInfo)
    * [Zone Database and Registry](#ZoneDatabaseAndRegistry)
    * [AtcZonedExtra](#AtcZonedExtra)
    * [AtcZoneRegistrar](#AtcZoneRegistrar)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Example"></a>
## Example

The expected usage is something like this:

```C
#include <acetimec.h>

AtcZoneProcessing los_angeles_processing;
AtcZoneProcessing new_york_processing;

// initialize the time zone processing workspace
void setup()
{
  atc_processing_init(&los_angeles_processing);
  atc_processing_init(&new_york_processing);
}

void do_something()
{
  atc_time_t seconds = 3432423;

  // convert epoch seconds to date/time components for given time zone
  AtcZonedDateTime zdt;
  int8_t err = atc_zoned_date_time_from_epoch_seconds(
    &los_angeles_processing,
    &kAtcZoneAmerica_Los_Angeles,
    seconds,
    &zdt);
  if (err) { /*error*/ }
  ...

  // convert zoned_date_time to epoch seconds
  seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
  if (seconds == kAtcInvalidEpochSeconds) { /*error*/ }
  ...

  // convert components to zoned_date_time
  AtcLocalDateTime ldt = { year, month, day, hour, minute, second };
  err = atc_zoned_date_time_from_local_date_time(
    &los_angeles_processing,
    &kAtcZoneAmerica_Los_Angeles,
    &ldt,
    0 /*fold*/,
    &zdt);
  if (err) { /*error*/ }
  ...

  // convert America/Los_Angles to America/New_York
  AtcZonedDateTime nydt;
  err = atc_zoned_date_time_from_local_date_time(
    &new_york_processing,
    &kAtcZoneAmerica_New_York,
    &zdt,
    &nydt);
  if (err) { /*error*/ }
}
```

<a name="Installation"></a>
## Installation

I am not familiar with any of the C language package managers. To obtain this
library, you should manually clone the project into an appropriate place on your
computer:

```C
$ git clone https://github.com/bxparks/AceTimeC
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

The functions in [epoch.h](src/ace_time_c/epoch.h) provide features related to
the epoch used by the AceTimeC library. By default, the epoch is 2050-01-01
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
`atc_set_current_epoch_year()` function, then the `atc_processing_init()`
function (see [AtcZoneProcessing](#AtcZoneProcessing)) must be called to
reinitialize any instance of `AtcZoneProcessing` that may have used a different
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

<a name="AtcLocalDate"></a>
### AtcLocalDate

The functions in [local_date.h](src/ace_time_c/local_date.h) provide features
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

The `AtcLocalDate` is used primarily for internal purposes, and is exposed
mostly for consistency with the other data structures described in later
sections.

```C
typedef struct AtcLocalDate
  int16_t year;
  uint8_t month;
  uint8_t day;
} AtcLocalDate;

void atc_local_date_increment_one_day(AtcLocalDate *ld);

void atc_local_date_decrement_one_day(AtcLocalDate *ld);
```

<a name="AtcLocalDateTime"></a>
### AtcLocalDateTime

The functions in [local_date_time.h](src/ace_time_c/local_date_time.h) operate
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
} AtcLocalDateTime;
```

There are 2 functions which operate on this data type:

```C
atc_time_t atc_local_date_time_to_epoch_seconds(
    const AtcLocalDateTime *ldt);

int8_t atc_local_date_time_from_epoch_seconds(
  atc_time_t epoch_seconds,
  AtcLocalDateTime *ldt);
```

The `atc_local_date_time_to_epoch_seconds()` function converts the given
`AtcLocalDateTime` into its `atc_time_t` epoch seconds. If an error occurs, the
function returns `kAtcInvalidEpochSeconds`.

The `atc_local_date_time_from_epoch_seconds()` function converts the given epoch
seconds into the `AtcLocalDateTime` components. If an error occurs, the function
returns `kAtcErrGeneric`, otherwise it returns `kAtcErrOk`.

<a name="AtcOffsetDateTime"></a>
### AtcOffsetDateTime

The functions in [offset_date_time.h](src/ace_time_c/offset_date_time.h) operate
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

  int16_t offset_minutes;
} AtcOffsetDateTime;
```

The memory layout of `AtcOffsetDateTime` was designed to be identical to
`AtcLocalDateTime` so that functions that accept a pointer to `AtcLocalDateTime`
can be given a pointer to `AtcOffsetDateTime` as well.

The `fold` parameter is used to disambiguate a time which occurs twice due
to a DST change. For example, in most of North America, the time zone switches
from DST to Standard time in the fall. The wall clock "falls back" from 2:00am
to 1:00am, which means that the time from 1:00am to 2:00am occurs twice. The
`fold` parameter is 0 for the first occurrence, and 1 for the second occurrence.

The meaning of the `fold` parameter is identical to the `fold` parameter in the
AceTime library, which itself borrowed the concept from the [PEP
495](https://www.python.org/dev/peps/pep-0495/) document in Python 3.6.

There are 2 functions that operate on the `AtcOffsetDateTime` object:

```C
atc_time_t atc_offset_date_time_to_epoch_seconds(
    const AtcOffsetDateTime *odt);

int8_t atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    AtcOffsetDateTime *odt);
```

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`AtcOffsetDateTime` into its `atc_time_t` epoch seconds, taking into account the
`offset_minutes` field. If an error occurs, the function returns
`kAtcInvalidEpochSeconds`. The `fold` parameter is ignored.

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`epoch_seconds` and `offset_minutes` into the `AtcOffsetDateTime` components. If
an error occurs, the function returns `kAtcErrGeneric`, otherwise it returns
`kAtcErrOk`. The `fold` parameter is ignored.

<a name="AtcZonedDateTime"></a>
### AtcZonedDateTime

The functions in [zoned_date_time.h](src/ace_time_c/zoned_date_time.h) operate
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

  int16_t offset_minutes; /* possibly ignored */
  const AtcZoneInfo *zone_info; /* nullable, possibly ignored */
} AtcZonedDateTime;
```

The memory layout of `AtcZonedDateTime` was designed to be identical to
`AtcOffsetDateTime`, so that functions that accept a pointer to
`AtcOffsetDateTime` can also accept pointers to `AtcZonedDateTime`.

The following functions operate on the `AtcZonedDateTime`:

```C
atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const AtcZonedDateTime *zdt);

int8_t atc_zoned_date_time_from_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcZonedDateTime *zdt);

int8_t atc_zoned_date_time_from_local_date_time(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    const AtcLocalDateTime *ldt,
    uint8_t fold,
    AtcZonedDateTime *zdt);

int8_t atc_zoned_date_time_convert(
    AtcZoneProcessing *dst_processing,
    const AtcZoneInfo *dst_zone_info,
    const AtcLocalDateTime *src,
    AtcZonedDateTime *dst);

int8_t atc_zoned_date_time_normalize(
    AtcZoneProcessing *processing,
    AtcZonedDateTime *zdt);
```

The `atc_zoned_date_time_to_epoch_seconds()` function converts the given
`AtcZonedDateTime` into its `atc_time_t` epoch seconds, taking into account the
time zone defined by the `zone_info` field inside the `AtcZonedDatetime`. If an
error occurs, the function returns `kAtcInvalidEpochSeconds`.

The `atc_offset_date_time_from_epoch_seconds()` function converts the given
`epoch_seconds` and `zone_info` into the `AtcZonedDateTime` components. If an
error occurs, the function returns `kAtcErrGeneric`, otherwise it returns
`kAtcErrOk`. The `fold` parameter is usually 0. However, during a DST shift
(described above), a `fold=0` indicates the first occurrence of the local wall
clock, and `fold=1` indicates the second occurrence of the local wall clock.

The `atc_zoned_date_time_from_local_date_time()` converts the date-time
components defined by the `AtcLocalDateTime` to the `AtcZonedDateTime`, taking
into account the time zone defined by `zone_info` and the `fold` parameter. In
most cases, the `fold` parameter has no effect. But for cases where a local wall
clock occurs twice (e.g. during a DST to Standard time shift), the `fold`
parameter disambiguates the multiple occurrence of the local time.

The `atc_zoned_date_time_convert()` function converts an `AtcZonedDateTime`
instance from one time zone to another. The `src` instance contains the original
time zone. The `dst` instance will contain the date-time of the time zone
represented by `dst_zone_info`.

The `fold` parameter occurs in 2 places, as an input parameter of one the above
functions, and as an output parameter in the `AtcZonedDateTime` data structure.
The `fold` parameter serves to disambiguate certain local date-time instances
where the time occurs twice, or does not exist at all.

For example, in the autumn in North America, the wall clock changes from 02:00
(DST) to 01:00 (Standard). This means means that the wall clock from 01:00 to
02:00 occur twice. In the spring time, the wall clock changes from 02:00 to
03:00, which means there is a gap where the times do not exist at all.

During a repeat:

* `fold=0` indicates the earlier of the 2 repeated time,
* `fold=1` indicates the later of the 2 repeated time.

During a gap:

* `fold=0` indicates that the *earlier* UTC offset should be used, which causes
  the effective epoch seconds to be the *later* one, which then gets normalized
  to the *later* `AtcZonedDateTime`.
* `fold=1` that the *later* UTC offset, which causes the effective epoch seconds
  to be the *earlier* one, which then gets normalized to the *earlier*
  `AtcZonedDateTime`.

These conventions are meant to be identical to the one described by the Python
[PEP 495](https://www.python.org/dev/peps/pep-0495/) document.

<a name="AtcZoneProcessing"></a>
### AtcZoneProcessing

The `AtcZoneProcessing` data structure provides a workspace for the various
internal functions that perform time zone calculations. The internal details
should be considered to be private and subject to change without notice. One of
this data type should be created statically for each time zone used by the
downstream application. (Another possibility is to create one on the heap at
startup time, then never freed.)

Each time zone should be assigned an instance of the `AtcZoneProcessing`. An
instance of `AtcZoneProcessing` should be initialized only once, usually at the
beginning of the application:

```C
AtcZoneProcessing los_angeles_processing;

void setup()
{
  atc_processing_init(&los_angeles_processing);
}
```

The `AtcZoneProcessing` instance keeps a cache of UTC offset transitions
spanning a year. Multiple calls to various `atc_zoned_date_time_XXX()` functions
with the same `AtcZoneProcessing` instance within a given year will execute much
faster than other years.

If memory is tight, an `AtcZoneProcessing` instance could be used by multiple
time zones (i.e. different `AtcZoneInfo`). However, each time the time zone
changes, the internal cache of the `AtcZoneProcessing` instance will be cleared
and recalculated, so the execution speed may decrease significantly.

**Warning**: If the epoch year is changed using the
`atc_set_current_epoch_year()` function (see [Epoch](#Epoch)), then the
`atc_processing_init()` function must be called to reinitialize any instance of
`AtcZoneProcessing` that may have used a different epoch year.

<a name="AtcZoneInfo"></a>
### AtcZoneInfo

The `AtcZoneInfo` data structure in [zone_info.h](src/ace_time_c/zone_info.h)
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

The Zone Database in the [src/ace_time_c/zonedb/](src/ace_time_c/zonedb)
directory contains the collection of `AtcZoneInfo` instances representing all
time zones defined by the IANA TZ database. The C code under `zonedb/` is
programmatically generated by scripts in the
[AceTimeTools](https://github.com/bxparks/AceTimeTools) project. For example,
the `America/Los_Angeles` time zone is represented by an instance of
`AtcZoneInfo` named `kAtcZoneAmerica_Los_Angeles`. A pointer to this data
structure should be passed into functions which expect a `const AtcZoneInfo *`
pointer.

The full list of Zones (and Links) supported by this library is given in the
[zonedb/zone_infos.h](src/ace_time_c/zonedb/zone_infos.h) header file, which is
automatically included by the `<acetimec.h>` header file.

The IANA TZ database is often updated to track changes to the DST rules in
different countries and regions. The version of the TZ database that was used to
generate the AceTimeC Zone database is given by:

```C
extern const char kAtcTzDatabaseVersion[];
```

For example, this string will be `"2022g"` for the 2022g version of the TZ
database.

The Zone Registry is defined in the
[zonedb/zone_registry.h](src/ace_time_c/zonedb/zone_registry.h) file which
contains the list of all Zones (and Links) supported by this library. It allows
us to locate the `AtcZoneInfo` pointer using the human readable zone name (e.g.
`"America/Los_Angeles"`) or its 32-bit zone identifier (e.g. `0xb7f7e8f2`).

As of TZDB 2022g, 2 zone registries are provided:

```C
// Zones
#define kAtcZoneRegistrySize 356
extern const AtcZoneInfo * const kAtcZoneRegistry[356];

// Zones and Links
#define kAtcZoneAndLinkRegistrySize 595
extern const AtcZoneInfo * const kAtcZoneAndLinkRegistry[595];
```

The `kAtcZoneRegistry` and `kAtcZoneAndLinkRegistry` are used by the [Zone
Registrar functions](#AtcZoneRegistrar) described below.

<a name="AtcZonedExtra"></a>
### AtcZonedExtra

The `AtcZonedExtra` structure in [zoned_extra.h](src/ace_time_c/zoned_extra.h)
holds additional meta information about a particular time zone, usually at a
particular epoch seconds:

```C
typedef struct AtcZonedExtra {
  int16_t std_offset_minutes; // STD offset
  int16_t dst_offset_minutes; // DST offset
  char abbrev[kAtcAbbrevSize];
} AtcZonedExtra;
```

There is one function that populates this type given an `epoch_seconds` and its
`zone_info`:

```C
int8_t atc_zoned_extra_from_epoch_seconds(
    AtcZoneProcessing *processing,
    const AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    AtcZonedExtra *extra);
```

This function returns `kAtcErrGeneric` if an error is encountered, otherwise it
returns `kAtcErrOk`.

<a name="AtcZoneRegistrar"></a>
## AtcZoneRegistrar

The functions in [zone_registrar.h](src/ace_time_c/zone_registrar.h) allow
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

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments, or feature requests for this library,
please use the [GitHub
Discussions](https://github.com/bxparks/AceTimeC/discussions) for this project.
If you have bug reports, please file a ticket in [GitHub
Issues](https://github.com/bxparks/AceTimeC/issues). Feature requests should go
into Discussions first because they often have alternative solutions which are
useful to remain visible, instead of disappearing from the default view of the
Issue tracker after the ticket is closed.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
