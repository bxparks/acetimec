/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_C_OFFSET_DATE_TIME_H
#define ACE_TIME_C_OFFSET_DATE_TIME_H

/**
 * Date and time fields with a UTC offset in minutes.
 *
 * In AceTime, the 'fold' parameter was included in the low-level LocalTime
 * class, which worked because C++ allows this extra parameter to be mostly
 * ignore until needed using a default parameter in the constructor to
 * automatically set 'fold' to a default value. Unfortunately, C does not have
 * default parameters, so adding a 'fold' in LocalDateTime causes unnecessary
 * friction. Therefore, we add this parameter at a higher level, in the
 * OffsetDateTime instead.
 */
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

#endif
