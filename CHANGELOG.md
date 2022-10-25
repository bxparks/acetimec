# Changelog

* Unreleased
* 0.3.0 (2022-08-30, TZDB 2022e)
    * Add `string_buffer.h` which implements a simple string buffer and
      provides a collection of print functions for converting various date
      and time structures into human readable forms such as ISO8601.
    * Add `atc_zoned_date_time_convert()` function for converting
      a date time from one time zone to another.
    * Create typedefs for `struct`, and remove unnecessary `struct` keyword.
    * Change `year` fields from `int8_t` to `int16_t`.
    * Add `atc_local_valid_year_lower()` and `atc_local_valid_year_upper()`
        * Defines the interval `lower <= year < upper` which is guaranteed
          to produce valid transitions.
    * Upgrade TZDB from 2022b to 2022e
        * 2022c
            * https://mm.icann.org/pipermail/tz-announce/2022-August/000072.html
                * Work around awk bug in FreeBSD, macOS, etc.
                * Improve tzselect on intercontinental Zones.
            * Skipped because there were no changes that affected AceTime.
        * 2022d
            * https://mm.icann.org/pipermail/tz-announce/2022-September/000073.html
                * Palestine transitions are now Saturdays at 02:00.
                * Simplify three Ukraine zones into one.
        * 2022e
            * https://mm.icann.org/pipermail/tz-announce/2022-October/000074.html
                * Jordan and Syria switch from +02/+03 with DST to year-round
                  +03.
* 0.2.0 (2022-08-30, TZDB 2022b)
    * Add doxygen docs. Add docstrings to various functions and structures to
      eliminate all doxygen warnings.
    * Add `atc_zone_info_zone_name()` and `atc_zone_info_short_name()` functions
      to extract the zone name from its `AtcZoneInfo`.
    * Bug fix: copy `AtcZoneInfo` into `AtcZonedDateTime` correctly.
    * Change return type of various functions from `bool` (true for success)
      to an `int8_t` error code (0 for success). Define `kAtcErrOk` and
      `kAtcErrGeneric` constants.
* 0.1.0 (2022-08-23, TZDB 2022b)
    * Upgrade to TZDB 2022b.
    * Add unit testing using bxparks/ACUnit.
    * Add support for (symbolic) Link entries.
    * Add `zone_registrar.c` to allow searching by zone name and zone id.`
    * Add complete set of unit tests derived from
      AceTime/ExtendedZoneProcessorTest.
* 0.0.0 (2022-04-08, TZDB 2022a)
    * Create project.
