/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

/**
 * @file local_time.h
 *
 * Low-level time functions.
 */

#include <stdint.h>

int32_t atc_local_time_to_seconds(uint8_t hour, uint8_t minute, uint8_t second);
