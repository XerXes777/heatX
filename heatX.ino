/**
 * @file heatX.ino
 *  \~english @brief Main implementation for heatX
 *  \~german @brief Hauptimplementierung für heatX
 *  \~english @details This file contains the main logic for controlling the heatX device.
 *  \~german @details Diese Datei enthält die Hauptlogik zur Steuerung des heatX-Geräts.
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

#include <SimpleRotary.h>
// #include "src/Waveshare_LCD1602_RGB.h"
#include "src/LiquidCrystal_AIP31068_I2C.h"


#include "src/globals_hx.h"
#include "src/gpio_hx.h"
#include "src/heating_hx.h"
#include "src/lcd_hx.h"
#include "src/pid_hx.h"
#include "src/sensor_hx.h"

SET_LOOP_TASK_STACK_SIZE(16 * 1024);  ///< Set loop task stack size to 16 KB


/* ============================================================================================= */
// LCD
/* ============================================================================================= */
void setupLcd();
void createLcdSymbol();
void setStaticHomeContent();
void updateHomeContent();

void callbackMaterialPreset(uint8_t pos);
void callbackTargetHeatTemp(int pos);
void callbackTargetHeatHum(int pos);
void callbackTargetHeatTime(int pos);
void callbackTargetHeatMode(int pos);

void callbackIntRange(int pos);
void callbackToggle(bool isOn);

/* ============================================================================================= */
// TEMPERATURE SENSOR (BME280)
/* ============================================================================================= */
void setupHeatSensor();
void checkHeatSensorStatus();

/* ============================================================================================= */
// HEATING
/* ============================================================================================= */
void setupHeating();
void controlHeating();
void controlFan(bool powerOn);
void setCountdownHeatTime();

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//
//                                   🧩 CLASS INSTANCES 🧩
//
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================================= */
// LCD
/* ============================================================================================= */
// Waveshare_LCD1602_RGB lcd(_LCD_COLS, _LCD_ROWS);
LiquidCrystal_AIP31068_I2C lcd(_LCD_ADDRESS, _LCD_COLS, _LCD_ROWS);

/* ============================================================================================= */
// BUTTON
/* ============================================================================================= */
ButtonActiveLow buttonStart(_PIN_START, _BUTTON_DEBOUNCE_TIME);
ButtonActiveLow buttonStop(_PIN_STOP, _BUTTON_DEBOUNCE_TIME);

/* ============================================================================================= */
// ENCODER
/* ============================================================================================= */
SimpleRotary encoder(_PIN_ENC_CLK, _PIN_ENC_DT, _PIN_ENC_BUTTON);

/* ============================================================================================= */
// TEMPERATURE SENSOR (BME280)
/* ============================================================================================= */
CustomBME280 bme;

/* ============================================================================================= */
// HEATING
/* ============================================================================================= */
GpioOffDelay fan(_PIN_FAN, _FAN_OFFDELAY);
GpioOffDelay fanHeat(_PIN_FAN_HEAT, _FAN_HEAT_OFFDELAY);

/* ============================================================================================= */
// PID CONTROLLER
/* ============================================================================================= */
PID_heatX pidHeating(
  _PID_TEMP_KP_PRESET,  // Proportional gain for fan speed PID
  _PID_TEMP_KI_PRESET,  // Integral gain for fan speed PID
  _PID_TEMP_KD_PRESET,  // Derivative gain for fan speed PID
  0);                   // 0 = Direct control

PID_heatX pidHum(
  _PID_HUM_KP_PRESET,  // Proportional gain for fan speed PID
  _PID_HUM_KI_PRESET,  // Integral gain for fan speed PID
  _PID_HUM_KD_PRESET,  // Derivative gain for fan speed PID
  0);                  // 0 = Direct control

PID_heatX pidFan(
  _PID_FAN_KP_PRESET,  // Proportional gain for fan speed PID
  _PID_FAN_KI_PRESET,  // Integral gain for fan speed PID
  _PID_FAN_KD_PRESET,  // Derivative gain for fan speed PID
  0);                  // 0 = Direct control

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//
//                                    🔧 SETUP FUNCTIONS 🔧
//
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void createLcdSymbol() {
  // Note: Symbols 0 and 1 are reserved for LcdMenu.

  // Degree Symbol
  uint8_t degreeSymbol[8] = {
    0b00111,
    0b00101,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcd.createChar(2, degreeSymbol);
  delay(_SETUP_DELAY);

  // Temperature Symbol (Thermometer)
  uint8_t tempSymbol[8] = {
    0b00100,
    0b01010,
    0b01010,
    0b01010,
    0b01110,
    0b11111,
    0b01110,
    0b00000
  };
  lcd.createChar(3, tempSymbol);
  delay(_SETUP_DELAY);

  // Humidity Symbol (Droplet)
  uint8_t humiditySymbol[8] = {
    0b00100,
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b00000,
    0b00000
  };
  lcd.createChar(4, humiditySymbol);
  delay(_SETUP_DELAY);

  // Time Symbol (Clock)
  uint8_t timeSymbol[8] = {
    0b01110,
    0b10001,
    0b10101,
    0b10101,
    0b10001,
    0b01110,
    0b00000,
    0b00000
  };
  lcd.createChar(5, timeSymbol);
  delay(_SETUP_DELAY);

  // Mode Symbol (Alternative Design)
  uint8_t modeSymbol[8] = {
    0b00100,
    0b01010,
    0b11111,
    0b01110,
    0b01110,
    0b11111,
    0b01010,
    0b00100
  };
  lcd.createChar(6, modeSymbol);
  delay(_SETUP_DELAY);

  // Free Slot Placeholder (Empty)
  uint8_t freeSymbol[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcd.createChar(7, freeSymbol);
  delay(_SETUP_DELAY);
}

void setupLcd() {
  // Copy material names to the array
  for (size_t i = 0; i < _MATERIAL_COUNT; ++i) {
    materialNames[i] = materialPresets[i].name;
  }

  lcd.init();

  createLcdSymbol();
  setStaticHomeContent();
  updateHomeContent();
}

void setupHeatSensor() {
  int attempts = 0;
  int allowedAttempts = 3;

  while (!bme.begin(_TEMPSENSOR_I2C_ADDRESS_1, &Wire) && attempts < allowedAttempts) {
    Serial.println("Not find BMx280");
    delay(1000);
    attempts++;
  }
  if (attempts >= allowedAttempts) {
    Serial.println("BMx280 error");
    delay(5000);
    return;
  }
  delay(_SETUP_DELAY);
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X4,  // temperature
                  Adafruit_BME280::SAMPLING_X4,  // pressure
                  Adafruit_BME280::SAMPLING_X4,  // humidity
                  Adafruit_BME280::FILTER_X16,
                  Adafruit_BME280::STANDBY_MS_125);

  delay(_SETUP_DELAY);
}

void setupHeating() {
  ledcAttach(_PIN_HEAT, _PWM_FREQUENCY, _PWM_RESOLUTION);

  pidHeating.SetOutputLimits(0, _PWM_MAX_VALUE);
  pidHeating.SetSetpoint(_TEMP_PRESET);
  pidHeating.SetSampleTime(150);
  // pidHeating.SetProportionalMode(0.0);  // Enable proportional on measurement
}

void setup() {
  setupLcd();
  setupSerial();
  setupDebug();
  setupHeatSensor();
  setupHeating();
}

void setStaticHomeContent() {

  // Temp actuel / target
  lcd.setCursor(0, 0);
  lcd.write(byte(3));  // Temperature Symbol
  lcd.print("   /   C");
  lcd.write(byte(2));  // Degree Symbol

  // Hum actuel / target
  lcd.setCursor((_LCD_COLS - 4), 0);
  lcd.write(byte(4));  // Humidity Symbol
  lcd.print("  %");

  // Heating mode
  lcd.setCursor(0, 1);
  // lcd.write(byte(6));  // Mode Symbol
  lcd.print("Box Heat");

  // Countdown time actual
  lcd.setCursor((_LCD_COLS - 7), 1);
  lcd.write(byte(5));  // Time Symbol
  lcd.print("  :  s");
}

void updateHomeContent() {
  // Temp actual
  lcd.setCursor(1, 0);
  lcd.printf("%3d", actualHeatingValue.temperature);

  // Temp target
  lcd.setCursor(5, 0);
  lcd.printf("%3d", targetHeatingValue.temperature);

  // Hum actual
  lcd.setCursor((_LCD_COLS - 3), 0);
  lcd.printf("%2d", actualHeatingValue.humidity);

  // Hum target
  // lcd.setCursor(13, 0);
  // lcd.printf("%2d", targetHeatingValue.humidity);

  // Countdown heat time actual
  setCountdownHeatTime();
  lcd.setCursor((_LCD_COLS - 6), 1);
  lcd.printf("%2d", actualCountdown.hours);
  lcd.setCursor((_LCD_COLS - 3), 1);
  lcd.printf("%2d", actualCountdown.minutes);
}

void setCountdownHeatTime() {
  actualCountdown.hours = targetCountdown.hours;
  actualCountdown.minutes = 0;
}

void checkHeatSensorStatus() {
  static unsigned long lastMillis;
  unsigned long currentMillis;
  static int sensorIsMeasuring;
  static int lastSensorMeasuringState;
  uint8_t status;

  currentMillis = millis();
  if (currentMillis - lastMillis >= 5) {
    lastMillis = currentMillis;

    uint8_t status = bme.readRegister(BME280_REGISTER_STATUS);
    sensorIsMeasuring = status & 0x08;  // Bit 3: Measuring
    digitalWrite(_PIN_DEBUG_CH4, sensorIsMeasuring);
    if (sensorIsMeasuring == !lastSensorMeasuringState) {

      digitalWrite(_PIN_DEBUG_CH5, HIGH);

      actualHeatingValue.temperature = bme.readTemperature();
      actualHeatingValue.humidity = bme.readHumidity();
      updateHomeContent();

      pidHeating.SetInput(actualHeatingValue.temperature);
      pidHeating.SetInput(map(analogRead(_PIN_DEBUG_POTI), 0, 4095, _TEMP_MIN, _TEMP_MAX));
      pidHeating.SetSetpoint(targetHeatingValue.temperature);
      // Serial.printf("Poti: %d\n", (analogRead(_PIN_DEBUG_POTI)));
      controlHeating();

      digitalWrite(_PIN_DEBUG_CH5, LOW);
    }
    lastSensorMeasuringState = sensorIsMeasuring;
  }
}

void controlHeating() {
  static int iHeaderCounter;
  static int HeatingRunning;
  bool HeatingIsOn;

  HeatingIsOn = (pidHeating.GetOutput() > 0.0);
  if (buttonStart.isPressed()) {
    HeatingRunning = 1;
  }
  if (buttonStop.isPressed()) {
    HeatingRunning = 0;
  }
  if (HeatingRunning) {
    pidHeating.SetMode(1);  // 1 = Automatic --> On

    if (pidHeating.Compute()) {
      digitalWrite(_PIN_DEBUG_CH6, HIGH);
      ledcWrite(_PIN_HEAT, pidHeating.GetOutput());

      Serial.printf("Set:%.2f In:%.2f Out:%.2f\n",
                    pidHeating.GetSetpoint(), pidHeating.GetInput(), mapFloat(pidHeating.GetOutput(), 0.0, 4095.0, 0.0, 100.0));

      digitalWrite(_PIN_DEBUG_CH6, LOW);
    }
  } else {
    ledcWrite(_PIN_HEAT, 0);
    HeatingIsOn = 0;
    pidHeating.SetMode(0);  // 0 = Manual --> Off
  }
  fan.control(HeatingIsOn);
  fanHeat.control(HeatingIsOn);
}

// LCD callbacks
void callbackToggle(bool isOn) {
  Serial.println(isOn);
}

void callbackIntRange(int pos) {
  Serial.println(pos);
}

void callbackTargetHeatTemp(int pos) {
  targetHeatingValue.temperature = pos;
  updateHomeContent();
}

void callbackTargetHeatHum(int pos) {
  targetHeatingValue.humidity = pos;
  updateHomeContent();
}

void callbackTargetHeatTime(int pos) {
  targetCountdown.hours = pos;
  updateHomeContent();
}

void callbackTargetHeatMode(int pos) {
  Serial.println(pos);
}

void callbackMaterialPreset(uint8_t pos) {
  // Check if the index is within the valid range
  if (pos < (_MATERIAL_COUNT)) {
    targetHeatingValue.temperature = materialPresets[pos].value;
    updateHomeContent();
    Serial.printf("New preset temp is: %d\n", materialPresets[pos].value);
  } else {
    Serial.printf("Error: Invalid material preset index: %d\n", pos);
  }
}

void loop() {
  buttonStart.update();
  buttonStop.update();
  fan.update();
  fanHeat.update();
  checkHeatSensorStatus();

  delay(5000);
  lcd.noDisplay();
  delay(3000);
  lcd.display();
}