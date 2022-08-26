# AceTime for C

[![ACUnit Tests](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml)

Time zone library for the C language, based on algorithms and techniques
from the [AceTime](https://github.com/bxparks/AceTime) library for the Arduino
programming environment.

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

**Version**: 0.1.0 (2022-08-23, TZDB version 2022b)

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
    * [AtcZonedExtra](#AtcZonedExtra)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Example"></a>
## Example

The expected usage is something like this:

```C
#include <acetimec.h>

struct AtcZoneProcessing los_angeles_processing;

void something() {
  // initialize the time zone processing workspace
  atc_processing_init(&los_angeles_processing);

  atc_time_t seconds = 3432423;

  // convert epoch seconds to date/time components for given time zone
  struct AtcZonedDateTime zdt;
  bool status = atc_zoned_date_time_from_epoch_seconds(
    &los_angeles_processing,
    &kAtcZoneAmerica_Los_Angeles,
    seconds,
    &zdt);
  if (! status) { /*error*/ }

  // convert zoned_date_time to epoch seconds
  seconds = atc_zoned_date_time_to_epoch_seconds(&zdt);
  if (seconds == kAtcInvalidEpochSeconds) { /*error*/ }

  // convert components to zoned_date_time
  status = atc_zoned_date_time_from_components(
    &los_angeles_processing,
    &kAtcZoneAmerica_Los_Angeles,
    year, month, day,
    hour, minute, second,
    fold,
    &zdt);
  if (! status) { /*error*/ }
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

* `kAtcInvalidYear`
    * `INT16_MIN` (-32736)
    * indicates an invalid year
* `kAtcInvalidEpochSeconds`
    * `INT32_MIN` (-2147483648)
    * indicates an invalid epoch seconds
* ISO Weekdays
    * `kAtcIsoWeekdayMonday` = 1
    * `kAtcIsoWeekdayTuesday`
    * `kAtcIsoWeekdayWednesday`
    * `kAtcIsoWeekdayThursday`
    * `kAtcIsoWeekdayFriday`
    * `kAtcIsoWeekdaySaturday`
    * `kAtcIsoWeekdaySunday` = 7

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
struct AtcLocalDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
```

There are 2 functions which operate on this data type:

```C
atc_time_t atc_local_date_time_to_epoch_seconds(
    const struct AtcLocalDateTime *ldt);

bool atc_local_date_time_from_epoch_seconds(
  atc_time_t epoch_seconds,
  struct AtcLocalDateTime *ldt);
```

<a name="AtcOffsetDateTime"></a>
### AtcOffsetDateTime

The `AtcOffsetDateTime` type represents a date-time with a fixed offset from
UTC:

```C
struct AtcOffsetDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int16_t offset_minutes;
};
```

The initial memory layout of `AtcOffsetDateTime` was designed to be identical to
`AtcLocalDateTime`.

The `fold` parameter is used to disambiguate a time which occurs twice due
to a DST change. For example, in most of North America, the time zone switches
from DST to Standard time in the fall. The wall clock "falls back" from 2:00am
to 1:00am, which means that the time from 1:00am to 2:00am occurs twice. The
`fold` parameter is 0 for the first occurrence, and 1 for the second occurrence.

There are 2 functions that operate on the `AtcOffsetDateTime` object:

```C
atc_time_t atc_offset_date_time_to_epoch_seconds(
    const struct AtcOffsetDateTime *odt);

bool atc_offset_date_time_from_epoch_seconds(
    atc_time_t epoch_seconds,
    int16_t offset_minutes,
    struct AtcOffsetDateTime *odt);
```

<a name="AtcZonedDateTime"></a>
### AtcZonedDateTime

An `AtcZonedDateTime` is an `AtcOffsetDateTime` which also contains a reference
to the TZDB time zone.

```C
struct AtcZonedDateTime {
  int16_t year;
  uint8_t month;
  uint8_t day;

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t fold;

  int16_t offset_minutes; /* possibly ignored */
  const struct AtcZoneInfo *zone_info; /* nullable, possibly ignored */
};
```

The initial memory layout of `AtcZonedDateTime` was designed to be identical to
`AtcOffsetDateTime`.

There are 4 functions which operate on the `AtcZonedDateTime`:

```C
bool atc_zoned_date_time_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedDateTime *zdt);

atc_time_t atc_zoned_date_time_to_epoch_seconds(
    const struct AtcZonedDateTime *zdt);

bool atc_zoned_date_time_from_components(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    int16_t year, uint8_t month, uint8_t day,
    uint8_t hour, uint8_t minute, uint8_t second,
    uint8_t fold,
    struct AtcZonedDateTime *zdt);

bool atc_zoned_date_time_normalize(
    struct AtcZoneProcessing *processing,
    struct AtcZonedDateTime *zdt);
```

<a name="AtcZoneProcessing"></a>
### AtcZoneProcessing

The `AtcZoneProcessing` data structure provides a workspace for the various
internal functions that perform time zone calculations. The internal details
should be considered to be private and subject to change without notice. One of
this data type should be created statically for each time zone used by the
downstream application. (Another possibility is to create one on the heap at
startup time, then never freed.)

<a name="AtcZoneInfo"></a>
### AtcZoneInfo

The `AtcZoneInfo` type represents the DST transition rules of a single time
zone. The `src/ace_time_c/zonedb/` directory contains the data collection for
all time zones as generated by the
[AceTimeTools](https://github.com/bxparks/AceTimeTools) scripts. For example,
the `America/Los_Angeles` time zone is represented by
`kAtcZoneAmerica_Los_Angeles`. A pointer to this data structure should be passed
into function which expect a `const struct AtcZoneInfo *` pointer.

<a name="AtcZonedExtra"></a>
### AtcZonedExtra

The `AtcZonedExtra` holds additional meta information about a particular time
zone, usually at a particular epoch seconds:

```C
struct AtcZonedExtra {
  int16_t std_offset_minutes; // STD offset
  int16_t dst_offset_minutes; // DST offset
  char abbrev[kAtcAbbrevSize];
};
```

There is one function that populates this type given an `epoch_seconds`:

```C
bool atc_zoned_extra_from_epoch_seconds(
    struct AtcZoneProcessing *processing,
    const struct AtcZoneInfo *zone_info,
    atc_time_t epoch_seconds,
    struct AtcZonedExtra *extra);
```

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
