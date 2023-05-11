# Changelog

* Unreleased
* 0.9.0 (2023-05-11, TZDB 2023c)
    * Replace `err` return value from most external functions if there is an
      out-parameter on the function signature (e.g. `AtcZonedDateTime`) that can
      be sets to an error state.
        * Simplifies the API with only a single place to check for errors.
        * Error conditions are now be detected using:
            * `atc_local_date_time_is_error()`
            * `atc_offset_date_time_is_error()`
            * `atc_zoned_date_time_is_error()`
            * `atc_zoned_extra_is_error()`
    * Change `atc_XXX_print()` functions to place `AtcStringBuffer` as first
      argument.
        * This is more consistent with the `fprintf()` function.
    * MemoryBenchmark
        * Remove Teensy 3.2
            * No longer recommended for new projects as of 2023-02-14
            * "PJRC recommends use of Teensy 4.0 / 4.1 for new projects. We do
              not believe supply of chips for Teensy 3.x is likely to ever fully
              recover. These chips are made with 90 nm silicon process. Most of
              the world's semiconductor fabs are focusing on 45 nm or smaller,
              leaving limited supply for older chips. We anticipate the cost of
              these chips is likely to increase as the supply continues to
              dwindle"
        * Add SAMD21 (Seeeduino XIAO) and SAMD51 (Adafruit ItsyBitsy M4).
* 0.8.0 (2023-04-13, TZDB 2023c)
    * Upgrade TZDB from 2023b to 2023c.
        * https://mm.icann.org/pipermail/tz-announce/2023-March/000079.html
            * "This release's code and data are identical to 2023a.  In other
              words, this release reverts all changes made in 2023b other than
              commentary, as that appears to be the best of a bad set of
              short-notice choices for modeling this week's daylight saving
              chaos in Lebanon."
    * Add support for plain UTC timezone in `AtcTimeZone`.
        * Create pre-defined `atc_time_zone_utc` instance.
    * Move `AtcZonedExtra` factory functions to `AtcTimeZone`.
        * Simplify initialization sequence of `AtcZoneProcessor` by channeling
          all factory operations through `AtcTimeZone`.
    * zonedbs
        * Create `zonedbtesting` for unit tests, using a limited number of
          zones, over only a limited [2000,10000] year range.
        * Create `zonedball` using [1800,10000] which covers the entire range of
          the TZDB database, for all zones.
        * Restrict range of `zonedb` to [2000,10000] again, to reduce size of
          database.
    * High resolution zonedb
        * Support one-second resolution for Zone.UNTIL and Rule.AT fields.
            * Enables support all zones before ~1972.
        * Support one-minute resolution for Zone.DSTOFF (i.e. Zone.RULES) and
          Rule.SAVE fields.
            * Handles a few zones around ~1930 whose DSTOFF is a 00:20 minutes,
            instead of a multiple of 00:15 minutes.
        * Extend year interval of `zonedb/` to `[1800,10000)`, which is
          slightly larger than the raw TZDB year range of `[1844,2087]` (as of
          TZDB 2022g).
        * Increases flash consumption by roughly 100%, 40kB to 80kB on 32-bit
          processors.
        * The old format still available through `-D ATC_HIRES_ZONEDB=0`.
        * Validation program `validate_against_libc/` shows that transitions
          are identical to the C libc library for all zones, for all years
          `[1800,2100)`.
    * MemoryBenchmark
        * Use my Arduino MemoryBenchmark infrastructure to extract flash and
          static memory consumption of the AceTimeC library across various
          microcontrollers.
        * The library does not support `PROGMEM` so AVR and ESP8266 processors
          consume excessive RAM.
    * Always generate anchor rules in zonedb.
        * Allows `zone_processor.c` to work over all years `[0,10000)`
          even with truncated zonedb (e.g. `[2000,2100)`).
        * Accuracy is guaranteed only for the requested interval (e.g.
          `[2000,2100)`.
        * But the code won't crash outside of that interval.
        * Extend range of `from_year`, `to_year`, `until_year` to +/-32767.
    * Zonedb Rule filtering
        * Use simplified ZoneRule filtering from AceTimeTools.
* 0.7.0 (2023-02-12, TZDB 2022g)
    * Links
        * Remove `kAtcLinkRegistry` and support for thin links.
        * Add `target_info` field to `AtcZoneInfo` to unify fat and symbolic
          links.
    * Replace `AtcZoneInfo.letter` with `letter_index`.
        * All letter fields are now indexes into `AtcZoneContext.letters` array,
          even the one-character letters.
    * New directory structure, similar to AceTimeGo library
        * `src/ace_time_c/` -> `src/acetimec/`
        * `src/ace_time_c/zone_info.h` -> `src/zoneinfo/*`
        * `src/ace_time_c/zone_info_utils.h` -> `src/zoneinfo/*`
        * `src/ace_time_c/zonedb/` -> `src/zonedb/`
    * Validation
        * Create `examples/validate_against_libc` to validate against the libc
          time functions.
    * examples/
        * Add `-D _GNU_SOURCE` flag to gain access to `gm_gmtoff` field
          in `struct tm`.
        * Allows validation of total UTC offset against the GNU libc library.
* 0.6.0 (2023-01-17, TZDB 2022g)
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
