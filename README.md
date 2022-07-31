# AceTime for C

[![MinUnit Tests](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml/badge.svg)](https://github.com/bxparks/AceTimeC/actions/workflows/unit_tests.yml)

Time zone library for the C language, based on algorithms and techniques
from [AceTime](https://github.com/bxparks/AceTime).

**Version**: 0.0.0 (2022-04-08, TZDB version 2022a)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

## Table of Contents

* [Installation](#Installation)
* [User Guide](#UserGuide)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Installation"></a>
## Installation

<a name="UserGuide"></a>
## User Guide

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
  atc_calc_date_time(
    &los_angeles_processing,
    seconds,
    &kAtcZoneAmerica_Los_Angeles,
    &zdt);

  // convert components to epoch seconds
  seconds = atc_calc_epoch_seconds(
    &los_angeles_processing, &zdt);
}
```

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments, or feature requests for this library,
please use the [GitHub
Discussions](https://github.com/bxparks/AceTime/discussions) for this project.
If you have bug reports, please file a ticket in [GitHub
Issues](https://github.com/bxparks/AceTime/issues). Feature requests should go
into Discussions first because they often have alternative solutions which are
useful to remain visible, instead of disappearing from the default view of the
Issue tracker after the ticket is closed.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
