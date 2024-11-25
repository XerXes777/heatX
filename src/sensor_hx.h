/**
 * @file sensor_hx.h
 * @brief Custom BME280 sensor class for efficient status polling.
 * @details This file contains the `CustomBME280` class, which extends the 
 *          Adafruit_BME280 library to allow direct register access, enabling 
 *          high-frequency polling of the `BME280_REGISTER_STATUS` register.
 * ### Changelog
 * - **2024-11-08**: Initial version created by Kevin Hinrichs
 *
 * @version 0.0.1
 * @date 2024-11-08
 * @author Kevin Hinrichs
 *
 * @copyright
 * Copyright (c) 2024 Kevin Hinrichs, Laurens Vaigt.  
 * All rights reserved. Unauthorized copying or use of this code is prohibited.
 */

#ifndef SENSOR_HX_H
#define SENSOR_HX_H

#include <Adafruit_BME280.h>

/**
 * @brief Custom BME280 sensor class for efficient status polling.
 * @details This class extends the `Adafruit_BME280` library to allow direct access 
 *          to the `readRegister` function, enabling efficient polling of the 
 *          `BME280_REGISTER_STATUS` register to detect when new sensor data is available.
 *
 * ### Use Case
 * The `readRegister` method allows high-frequency polling of the BME280 sensor's 
 * status register (e.g., bit 3: Measuring) to determine precisely when new data 
 * becomes available. The polling interval is managed to balance responsiveness 
 * and system efficiency.
 */
class CustomBME280 : public Adafruit_BME280 {
public:
  /**
   * @brief Reads a specified register from the BME280 sensor.
   * @details This function is intended for high-frequency polling of registers, 
   *          such as `BME280_REGISTER_STATUS` to monitor the sensor's state.
   * @param reg The register address to read.
   * @return The value read from the specified register.
   *
   * ### Example Usage
   * ```cpp
   * CustomBME280 sensor;
   * static unsigned long lastMillis = 0;
   * unsigned long currentMillis;
   * static int sensorIsMeasuring = 0;
   * static int lastSensorMeasuringState = 0;
   * 
   * void setup() {
   *   Serial.begin(9600);
   *   sensor.begin(0x76); // Initialize with the sensor's I2C address
   * }
   * 
   * void loop() {
   *   currentMillis = millis();
   *   if (currentMillis - lastMillis >= 5) { // Poll every 5 ms
   *     lastMillis = currentMillis;
   * 
   *     uint8_t status = sensor.readRegister(BME280_REGISTER_STATUS);
   *     sensorIsMeasuring = status & 0x08;  // Bit 3: Measuring
   * 
   *     if (sensorIsMeasuring && !lastSensorMeasuringState) {
   *       Serial.println("New measurement started.");
   *     } else if (!sensorIsMeasuring && lastSensorMeasuringState) {
   *       Serial.println("Measurement complete, new data available.");
   *     }
   * 
   *     lastSensorMeasuringState = sensorIsMeasuring;
   *   }
   * }
   * ```
   */
  uint8_t readRegister(uint8_t reg) {
    return read8(reg);
  }
};


#endif  // SENSOR_HX_H