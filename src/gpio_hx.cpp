/**
 * @file gpio_hx.cpp
 * @brief ...
 * @details ...
 *
 * ### Changelog
 * - **2024-11-08**: Initial version created by Kevin Hinrichs
 *
 * @version 0.0.1
 * @date 2024-11-08
 * @author Kevin Hinrichs
 *
 * @copyright
 * Copyright (c) 2024 Kevin Hinrichs, Laurens Vaigt.
 * Licensed under the MIT License. See the
 * <a href="LICENSE" target="_blank">LICENSE</a> file for details.
 */

#include "gpio_hx.h"
#include "globals_hx.h"

void setupDebug() {
  pinMode(_PIN_DEBUG_CH4, OUTPUT);
  pinMode(_PIN_DEBUG_CH5, OUTPUT);
  pinMode(_PIN_DEBUG_CH6, OUTPUT);
  pinMode(_PIN_DEBUG_CH7, OUTPUT);

  digitalWrite(_PIN_DEBUG_CH4, LOW);
  digitalWrite(_PIN_DEBUG_CH5, LOW);
  digitalWrite(_PIN_DEBUG_CH6, LOW);
  digitalWrite(_PIN_DEBUG_CH7, LOW);
}

void setupSerial() {
  Serial.begin(_SERIAL_BAUD);
  delay(_SETUP_DELAY);
  Serial.printf("\n\n\nTerminal on\n");
  delay(_SETUP_DELAY);
}