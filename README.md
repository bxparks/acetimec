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
the documentation provbided by the README.md and USER_GUIDE.md documents of the
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
    * [AtcLocalDateTime](#AtcLocalDateTime)
    * [AtcOffsetDateTime](#AtcOffsetDateTime)
    * [AtcZonedDateTime](#AtcZonedDateTime)
    * [AtcZoneProcessing](#AtcZoneProcessing)
    * [AtcZoneInfo](#AtcZoneInfo)
    * [Zone Database](#ZoneDatabase)
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
epoch of this library. That epoch will normally be 2000-01-01 00:00:00 UTC,
instead of the POSIX standard of 1970-01-01 00:00:00 UTC. That means that
largest date that can be represented by `atc_time_t` is 2068-01-19 03:14:07 UTC.

<a name="AtcLocalDateTime"></a>
### AtcLocalDateTime

The `AtcLocalDateTime` type represents the wall time, without any reference
to a time zone:

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

The `AtcOffsetDateTime` type represents a date-time with a fixed offset from
UTC:

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

The initial memory layout of `AtcOffsetDateTime` was designed to be identical to
`AtcLocalDateTime`.

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

An `AtcZonedDateTime` is an `AtcOffsetDateTime` which also contains a reference
to the TZDB time zone.

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

The initial memory layout of `AtcZonedDateTime` was designed to be identical to
`AtcOffsetDateTime`.

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

<a name="AtcZoneInfo"></a>
### AtcZoneInfo

The `AtcZoneInfo` data structure, defined in the
[zone_info.h](src/ace_time_c/zone_info.h) header file, defines the DST
transition rules of a single time zone. The pointer to the `AtcZoneInfo` is
meant to be passed around as opaque object for the most part since most of the
fields are meant for internal consumption. There are 3 accessor functions which
may be useful for end-users:

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

<a name="ZoneDatabase"></a>
### Zone Database

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
automatically included by the `acetime.h` header file.

The IANA TZ database is often updated to track changes to the DST rules in
different countries and regions. The version of the TZ database that was used to
generate the AceTimeC Zone database is given by:

```C
extern const char kAtcTzDatabaseVersion[];
```

For example, this string will be `"2022b"` for the 2022b version of the TZ
database.

<a name="AtcZonedExtra"></a>
### AtcZonedExtra

The `AtcZonedExtra` holds additional meta information about a particular time
zone, usually at a particular epoch seconds:

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

The Zone Registry is the list of all Zones (and Links) supported by this
library. It allows us to locate the `AtcZoneInfo` pointer using the
human readable zone name (e.g. `"America/Los_Angeles"`) or its 32-bit
zone identifier (e.g. `0xb7f7e8f2`).

As of TZDB 2022b, 2 zone registries are provided:

```C
// Zones
#define kAtcZoneRegistrySize 356
extern const AtcZoneInfo * const kAtcZoneRegistry[356];

// Zones and Links
#define kAtcZoneAndLinkRegistrySize 595
extern const AtcZoneInfo * const kAtcZoneAndLinkRegistry[595];
```

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

To retrieve the `AtcZoneInfo` pointer from the human readable zone name, the
code looks something like this:

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

void retrieve_zone_info_by_id()
{
  uint32_t zone_id = kAtcZoneIdAmerica_Los_Angeles;
  const AtcZoneInfo *zone_info = atc_registrar_find_by_id(&registrar, zone_id);
  if (zone_info == NULL) { /*error*/ }
  ...
}
```

The zone identifier is a unique and stable 32-bit integer associated for each
time zone. It was defined in the AceTime library. See the section
[CreateForZoneId](https://github.com/bxparks/AceTime/blob/develop/USER_GUIDE.md#CreateForZoneId)
in the `USER_GUIDE.md` for AceTime. A 32-bit integer is often more convenient in
an embedded environment than the human-readable zone name because the integer is
a fixed size and can be stored and retrieved quickly.

For example, instead of using the string `"America/Los_Angeles"`, this library
defines the 32-bit number `kAtcZoneIdAmerica_Los_Angeles=0xb7f7e8f2` for this
zone. This zone identifier can be passed into the `atc_registrar_find_by_id()`
function to retrieve the corresponding `AtcZoneInfo` object.

The `atc_registrar_init()` function determines whether or not the registry is
sorted by zone id. If it is, then the search functions (both `find_by_name()`
and `find_by_id()`) will use binary search algorithm. If the registry is not
sorted, then the search functions performs a linear search through the registry.
The binary search algorithm is `O(log(N))` and the linear search of `O(N)`.
The binary search will be far faster than the linear search if the registry
contains more than about 5-10 entries.

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
