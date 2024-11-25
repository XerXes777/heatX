/**
 * @file globals_hx.h
 * @brief Static project information for heatX
 * @details This file contains the all the settings, macros and prototypes
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

#ifndef GLOBALS_HX_H
#define GLOBALS_HX_H

#include <Arduino.h>

/**
 * @defgroup GPIO_Config GPIO Pin Configuration
 * @brief Centralized GPIO pin configuration for the entire project.
 * @{
 */

/** @defgroup PIN_Debug Debugging Pins
 * @brief GPIO pins assigned for debugging purposes.
 * @{
 */
#define _PIN_DEBUG_CH0 1   ///< GPIO pin for debug
#define _PIN_DEBUG_CH1 2   ///< GPIO pin for debug
#define _PIN_DEBUG_CH2 4   ///< GPIO pin for debug
#define _PIN_DEBUG_CH3 5   ///< GPIO pin for debug
#define _PIN_DEBUG_CH4 6   ///< GPIO pin for debug
#define _PIN_DEBUG_POTI 7  ///< GPIO pin for debug
/** @} */

/** @defgroup PIN_I2C I²C Communication Pins
 * @brief GPIO pins assigned for I²C communication.
 * @{
 */
#define _PIN_I2C_SDA 8  ///< GPIO pin for I²C SDA (data line)
#define _PIN_I2C_SCL 9  ///< GPIO pin for I²C SCL (clock line)
/** @} */

/** @defgroup PIN_SPI SPI Communication Pins
 * @brief GPIO pins assigned for SPI communication.
 * @{
 */
#define _PIN_SPI2_CS 10    ///< GPIO pin for SPI2 CS
#define _PIN_SPI2_MOSI 11  ///< GPIO pin for SPI2 MOSI
#define _PIN_SPI2_CLK 12   ///< GPIO pin for SPI2 CLK
#define _PIN_SPI2_MISO 13  ///< GPIO pin for SPI2 MISO
/** @} */

/** @defgroup PIN_Control Control Pins
 * @brief GPIO pins assigned for user input controls.
 * @{
 */
#define _PIN_START 14  ///< GPIO pin for the START button
#define _PIN_STOP 15   ///< GPIO pin for the STOP button

#define _PIN_ENC_BUTTON 16  ///< GPIO pin for encoder button
#define _PIN_ENC_B 17       ///< GPIO pin for encoder B
#define _PIN_ENC_A 18       ///< GPIO pin for encoder A
/** @} */

/** @defgroup PIN_Output Output Pins
 * @brief GPIO pins assigned for output devices.
 * @{
 */
#define _PIN_HEATING 21  ///< GPIO pin for heating control
#define _PIN_FAN 38      ///< GPIO pin for fan control
#define _PIN_BUZZER 47   ///< GPIO pin for buzzer
#define _PIN_RGB_LED 48  ///< GPIO pin for RGB LED (requires 5V solder joint!)
/** @} */

/** @} */  // End of GPIO_Config

/**
 * @defgroup Peripheral_Config Peripheral Configuration
 * @brief Configuration for connected peripherals, such as sensors.
 * 
 * This section defines addresses and other settings for peripherals
 * connected to the project.
 * @{
 */

/**
 * @defgroup LCD_Config LCD Display Configuration
 * @brief Macros for configuring the LCD display.
 * @{
 */
#define _LCD_I2C_ADDRESS 0x27  ///< I²C address for the LCD
#define _LCD_ROWS 4            ///< Number of rows on the LCD
#define _LCD_COLS 20           ///< Number of columns on the LCD
/** @} */

/**
 * @defgroup Serial_Config Serial Communication Configuration
 * @brief Macros for configuring the serial communication interface.
 * @{
 */
#define _SERIAL_BAUD 115200  ///< Baud rate for serial communication
/** @} */

/**
 * @defgroup Encoder_Config Encoder Configuration
 * @brief Macros for configuring the rotary encoder.
 * @{
 */
#define _ENC_CYCLES 20  ///< Number of cycles per encoder rotation
/** @} */

/**
 * @defgroup Button_Config Button Configuration
 * @brief Macros for configuring button behavior.
 * @{
 */
#define _BUTTON_DEBOUNCE_TIME 1  ///< Button debounce time in milliseconds
/** @} */

/**
 * @defgroup Temperature_Sensor Temperature Sensor Configuration
 * @brief Configuration for the BME280 temperature sensor.
 * @{
 */
#define _TEMPSENSOR_I2C_ADDRESS_1 0x76  ///< I2C first address for the BME280 sensor
#define _TEMPSENSOR_I2C_ADDRESS_2 0x77  ///< I2C sec address for the BME280 sensor
#define SEALEVELPRESSURE_HPA (1013.25)  ///< Standard sea level pressure in hPa
/** @} */

/**
 * @defgroup Heating_Config Heating Configuration
 * @brief Macros for temperature, humidity, and fan control settings.
 * @{
 */
#define _PWM_FREQUENCY 5000                          ///< PWM frequency in Hz
#define _PWM_RESOLUTION 12                           ///< PWM resolution in bits
#define _PWM_MAX_VALUE ((1 << _PWM_RESOLUTION) - 1)  ///< Maximum PWM value

#define _TEMP_MIN 40            ///< Minimum temperature in °C
#define _TEMP_MAX 80            ///< Maximum temperature in °C
#define _TEMP_PRESET _TEMP_MAX  ///< Default preset temperature

#define _HUM_MIN 0      ///< Minimum humidity in %
#define _HUM_MAX 60     ///< Maximum humidity in %
#define _HUM_PRESET 10  ///< Default preset humidity in %

#define _FAN_MIN 25     ///< Minimum fan power in %
#define _FAN_MAX 80     ///< Maximum fan power in %
#define _FAN_PRESET 50  ///< Default preset fan power in %

#define _TIME_MIN 1     ///< Minimum time in hours
#define _TIME_MAX 72    ///< Maximum time in hours
#define _TIME_PRESET 5  ///< Default preset time in hours

#define _FAN_OFFDELAY 3000  ///< Fan delay time in milliseconds
/** @} */

/**
 * @defgroup PID_Config PID Controller Configuration
 * @brief PID tuning parameters for temperature, humidity, and fan control.
 * @{
 */
#define _PID_TEMP_KP_PRESET 2.0  ///< Proportional gain for temperature control
#define _PID_TEMP_KI_PRESET 5.0  ///< Integral gain for temperature control
#define _PID_TEMP_KD_PRESET 1.0  ///< Derivative gain for temperature control

#define _PID_HUM_KP_PRESET 2.0  ///< Proportional gain for humidity control
#define _PID_HUM_KI_PRESET 5.0  ///< Integral gain for humidity control
#define _PID_HUM_KD_PRESET 1.0  ///< Derivative gain for humidity control

#define _PID_FAN_KP_PRESET 1.5  ///< Proportional gain for fan control
#define _PID_FAN_KI_PRESET 4.0  ///< Integral gain for fan control
#define _PID_FAN_KD_PRESET 0.5  ///< Derivative gain for fan control
/** @} */

/** @} */  // End of Peripheral_Config

/**
 * @defgroup System_Config System Configuration
 * @brief General system-level configuration macros.
 * @{
 */
#define _SETUP_DELAY 70  ///< Delay during setup in milliseconds
/** @} */

/**
 * @defgroup Material_Config Material Preset Configuration
 * @brief Temperature presets for different materials.
 * @{
 */
#define _PLA_PRESET 50     ///< Preset temperature for PLA (°C)
#define _PETG_PRESET 60    ///< Preset temperature for PETG (°C)
#define _ASA_PRESET 65     ///< Preset temperature for ASA (°C)
#define _ABS_PRESET 70     ///< Preset temperature for ABS (°C)
#define _PP_PRESET 70      ///< Preset temperature for PP (°C)
#define _PA_PRESET 70      ///< Preset temperature for PA (°C)
#define _PC_PRESET 70      ///< Preset temperature for PC (°C)
#define _TPU_PRESET 50     ///< Preset temperature for TPU (°C)
#define _MATERIAL_COUNT 8  ///< Total number of material presets.
/** @} */

/**
 * @defgroup Menu_Config Menu Configuration
 * @brief Configuration for menu navigation.
 * @{
 */
#define _MENU_HOME_PAGE 1   ///< Home page ID for menu
#define _MENU_SETUP_PAGE 2  ///< Setup page ID for menu
/** @} */

/** Heat control modes. */
enum enumHeatMode {
  TEMP_CONTROL,  ///< Mode for temperature control.
  HUM_CONTROL    ///< Mode for humidity control.
};

/** Menu states for the user interface. */
enum enumMenuState {
  MENU_HOME,      ///< Home page.
  MENU_SETTINGS,  ///< Settings page.
  MENU_INFO       ///< Info page.
};

/** 
 * @brief Represents the state of the menu.
 */
typedef struct {
  int currentPage;  ///< Current menu page.
  int totalPages;   ///< Total number of pages.
} MenuState;

/** 
 * @brief Holds data from the BME280 sensor.
 */
typedef struct {
  float temperature;  ///< Current temperature reading (°C).
  float humidity;     ///< Current humidity reading (%).
  float press;        ///< Current pressure reading (hPa).
  float altitude;     ///< Current altitude reading (meters).
  bool isActive;      ///< Status flag indicating if the sensor is active.
} SensorData;


/** 
 * @brief Contains target values for heating and humidity control.
 */
typedef struct {
  int temperature;  ///< Target temperature (°C).
  int humidity;     ///< Target humidity (%).
} HeatingValues;

/** 
 * @brief Represents a countdown timer.
 */
typedef struct {
  int hours;    ///< Countdown hours.
  int minutes;  ///< Countdown minutes.
} Countdown;

/** 
 * @brief Represents a material preset for heating configuration.
 */
typedef struct {
  const char *name;  ///< Material name (e.g., "PLA", "ABS").
  int value;         ///< Preset temperature value (°C).
} MaterialPreset;

/**
 * @brief Holds the current heating values (temperature and humidity).
 * @details This variable is used to track the actual temperature and humidity values in real-time.
 */
extern HeatingValues actualHeatingValue;

/**
 * @brief Tracks the current countdown timer.
 * @details This variable is used to store the remaining time for the heating operation.
 */
extern Countdown actualCountdown;

/**
 * @brief Holds the target heating values (temperature and humidity).
 * @details This variable stores the desired setpoints for temperature and humidity.
 */
extern HeatingValues targetHeatingValue;

/**
 * @brief Tracks the target countdown timer.
 * @details This variable is used to store the desired countdown duration for heating.
 */
extern Countdown targetCountdown;

/**
 * @brief Array of material presets for heating profiles.
 * @details Each entry contains a material name and its corresponding heating preset values.
 *
 * ### Materials:
 * - `"PLA"`: Preset for PLA material.
 * - `"PETG"`: Preset for PETG material.
 * - `"ASA"`: Preset for ASA material.
 * - `"ABS"`: Preset for ABS material.
 * - `"PP"`: Preset for PP material.
 * - `"PA"`: Preset for PA material.
 * - `"PC"`: Preset for PC material.
 * - `"TPU"`: Preset for TPU material.
 */
extern MaterialPreset materialPresets[_MATERIAL_COUNT];

/**
 * @brief Array of material names derived from `materialPresets`.
 * @details This array is populated for use in an LCD item list, containing the names of all supported materials.
 */
extern String materialNames[_MATERIAL_COUNT];

/**
 * @brief Maps a float value from one range to another.
 * @details Similar to the Arduino `map()` function, but supports floating-point numbers.
 * 
 * @param x Input value to map.
 * @param in_min Lower bound of the input range.
 * @param in_max Upper bound of the input range.
 * @param out_min Lower bound of the output range.
 * @param out_max Upper bound of the output range.
 * @return The mapped value in the output range.
 */
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);


#endif  // GLOBALS_HX_H