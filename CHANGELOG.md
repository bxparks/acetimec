# Changelog

* Unreleased
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
