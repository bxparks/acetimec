# Changelog

* Unreleased
    * Migrate to ACUnit v0.1.0.
    * `atc_days_to_current_epoch_from_converter_epoch`
        * Fix incorrect initial value.
    * `atc_date_tuple_subtract()`
        * Fix overflow bug.
    * `zonedb`
        * Rename `kAtcPolicyXxx` to `kAtcZonePolicyXxx` for consistency.
    * `zone_processor.h`
        * Renamed from `zone_processing.h`.
        * Rename `AtcZoneProcessing` to `AtcZoneProcessor`.
        * Incorporate same algorithm as AceTime
        * Unify `find_by_epochseconds()` and `find_by_local_date_time()` using a
          common `FindResult`.
        * Fix handling of input and output `fold` parameters during overlap in
          `find_by_local_date_time()`.
    * `AtcLocalDateTime`
        * Incorporate `fold` parameter, for consistency with `AtcOffsetDateTime`
          and `AtcZonedDateTime`.
        * Removes the explicit `uint8_t fold` argument from a number of
          functions, simplifying their usage.
        * More consistent with the AceTime library.
    * `AtcZonedExtra`
        * Add requested STD offset and DST offset, information which was
          otherwise lost during a DST gap.
        * Add `type` parameter to indicate exact match, gap, or overlap.
    * `AtcTimeZone`
        * Add `AtcTimeZone` struct to simplify arguments to various
          `atc_zoned_date_time*()` and `atc_zoned_extra*()` functions
    * printing to `AtcStringBuffer`
        * Change order of various `atc_xxx_print()` functions to place the
          date-time objects first, like the `this` pointer of an object.
* 0.5.0 (2022-12-04, TZDB 2022g)
    * Upgrade to TZDB 2022g
    * Add `extern "C"` to all header files.
    * Rename `string_buffer.h` functions to `atc_buf_init()` and
      `atc_buf_close()`. Add `atc_buf_reset()`.
    * Create `AtcZoneRegistrar` data structure for the various
      `atc_registrar_xxx()` functions.
    * Add `examples/arduino/HelloTextClock` to test compilation under Arduino
      environment.
* 0.4.0 (2022-11-04, TZDB 2022f)
    * Configurable current epoch year
        * Rename `atc_set_local_epoch_year()` and `atc_get_local_epoch_year()`
          to `atc_set_current_epoch_year()` and `atc_get_current_epoch_year()`,
          consistent with AceTime library.
        * Rename `atc_local_valid_year_lower()` and
          `atc_local_valid_year_upper()` to `atc_epoch_valid_year_lower()` and
          `atc_epoch_valid_year_uppper()`, consistent with AceTime library.
        * Create `epoch.h` for features related to current epoch year, and
          epoch day converters.
    * Upgrade TZDB from 2022e to 2022f
        * https://mm.icann.org/pipermail/tz-announce/2022-October/000075.html
			* Mexico will no longer observe DST except near the US border.
			* Chihuahua moves to year-round -06 on 2022-10-30.
			* Fiji no longer observes DST.
			* Move links to 'backward'.
			* In vanguard form, GMT is now a Zone and Etc/GMT a link.
			* zic now supports links to links, and vanguard form uses this.
			* Simplify four Ontario zones.
			* Fix a Y2438 bug when reading TZif data.
			* Enable 64-bit time_t on 32-bit glibc platforms.
			* Omit large-file support when no longer needed.
			* In C code, use some C23 features if available.
			* Remove no-longer-needed workaround for Qt bug 53071.
    * Add skeleton `libraries.properties` to test the C library with an
      Arduino board.
* 0.3.0 (2022-08-30, TZDB 2022e)
    * Add `string_buffer.h` which implements a simple string buffer and
      provides a collection of print functions for converting various date
      and time structures into human readable forms such as ISO8601.
    * Add `atc_zoned_date_time_convert()` function for converting
      a date time from one time zone to another.
    * Create typedefs for `struct`, and remove unnecessary `struct` keyword.
    * Change `year` fields from `int8_t` to `int16_t`.
    * Adjustable current epoch year
        * Add `atc_set_local_epoch_year()` and `atc_get_local_epoch_year()`
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
