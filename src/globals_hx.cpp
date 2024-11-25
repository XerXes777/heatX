/**
 * @file globals_hx.cpp
 * @brief Implementation of global variables for the heatX project.
 * @details This file contains the definitions of global variables 
 * used across the heatX project, such as heating values, countdown timers, 
 * and material presets.
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

#include "globals_hx.h"

HeatingValues actualHeatingValue = { 0, 0 };

Countdown actualCountdown = { 0, 0 };

HeatingValues targetHeatingValue = {
  _TEMP_PRESET, /**< Default target temperature. */
  _HUM_PRESET   /**< Default target humidity. */
};

Countdown targetCountdown = {
  _TIME_PRESET, /**< Default target hours. */
  0             /**< Default target minutes. */
};

MaterialPreset materialPresets[_MATERIAL_COUNT] = {
  { "PLA", _PLA_PRESET },   /**< Index 0: PLA preset. */
  { "PETG", _PETG_PRESET }, /**< Index 1: PETG preset. */
  { "ASA", _ASA_PRESET },   /**< Index 2: ASA preset. */
  { "ABS", _ABS_PRESET },   /**< Index 3: ABS preset. */
  { "PP", _PP_PRESET },     /**< Index 4: PP preset. */
  { "PA", _PA_PRESET },     /**< Index 5: PA preset. */
  { "PC", _PC_PRESET },     /**< Index 6: PC preset. */
  { "TPU", _TPU_PRESET }    /**< Index 7: TPU preset. */
};

String materialNames[_MATERIAL_COUNT];

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}