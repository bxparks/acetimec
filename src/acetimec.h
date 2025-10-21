/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_H
#define ACE_TIME_C_H

/* Version format: xxyyzz == "xx.yy.zz" */
#define ACE_TIME_C_VERSION 1400
#define ACE_TIME_C_VERSION_STRING "0.14.0"

#include "zoneinfo/zone_info.h"
#include "zoneinfo/zone_info_utils.h"
#include "acetimec/string_buffer.h"
#include "acetimec/epoch.h"
#include "acetimec/plain_date.h"
#include "acetimec/plain_time.h"
#include "acetimec/plain_date_time.h"
#include "acetimec/offset_date_time.h"
#include "acetimec/transition.h"
#include "acetimec/zone_processor.h"
#include "acetimec/time_zone.h"
#include "acetimec/zoned_date_time.h"
#include "acetimec/zone_registrar.h"
#include "acetimec/zoned_extra.h"

#if ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_HIGH
  #include "zonedball/zone_infos.h"
  #include "zonedball/zone_policies.h"
  #include "zonedball/zone_registry.h"
  #include "zonedb2000/zone_infos.h"
  #include "zonedb2000/zone_policies.h"
  #include "zonedb2000/zone_registry.h"
  #include "zonedb2025/zone_infos.h"
  #include "zonedb2025/zone_policies.h"
  #include "zonedb2025/zone_registry.h"
  #include "zonedbtesting/zone_infos.h"
  #include "zonedbtesting/zone_policies.h"
  #include "zonedbtesting/zone_registry.h"
#elif ACE_TIME_C_ZONEDB_RES == ACE_TIME_C_ZONEDB_RES_MID
  #include "zonedb2000/zone_infos.h"
  #include "zonedb2000/zone_policies.h"
  #include "zonedb2000/zone_registry.h"
  #include "zonedb2025/zone_infos.h"
  #include "zonedb2025/zone_policies.h"
  #include "zonedb2025/zone_registry.h"
  #include "zonedbtesting/zone_infos.h"
  #include "zonedbtesting/zone_policies.h"
  #include "zonedbtesting/zone_registry.h"
#else
  #error "Unknown ACE_TIME_C_ZONEDB_RES"
#endif

#endif
