// This file was generated by the following script:
//
//   $ /home/brian/src/AceTimeTools/src/acetimetools/tzcompiler.py
//     --input_dir /home/brian/src/AceTimeC/src/zonedb/tzfiles
//     --output_dir /home/brian/src/AceTimeC/src/zonedb
//     --tz_version 2022g
//     --action zonedb
//     --language c
//     --scope extended
//     --generate_int16_years
//     --start_year 2000
//     --until_year 10000
//     --nocompress
//
// using the TZ Database files
//
//   africa
//   antarctica
//   asia
//   australasia
//   backward
//   etcetera
//   europe
//   northamerica
//   southamerica
//
// from https://github.com/eggert/tz/releases/tag/2022g
//
// Supported Zones: 596 (351 zones, 245 links)
// Unsupported Zones: 0 (0 zones, 0 links)
// Original Years: [1844,2087]
// Generated Years: [1943,2087]
//
// DO NOT EDIT

#ifndef ACE_TIME_C_ZONEDBX_ZONE_REGISTRY_H
#define ACE_TIME_C_ZONEDBX_ZONE_REGISTRY_H

#include "../zoneinfo/zone_info.h"

#ifdef __cplusplus
extern "C" {
#endif

// Zones
#define kAtcZoneRegistrySize 351
extern const AtcZoneInfo * const kAtcZoneRegistry[351];

// Zones and Links
#define kAtcZoneAndLinkRegistrySize 596
extern const AtcZoneInfo * const kAtcZoneAndLinkRegistry[596];

#ifdef __cplusplus
}
#endif

#endif