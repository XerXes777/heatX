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

#include <ItemIntRange.h>
#include <ItemList.h>
#include <ItemToggle.h>
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <SimpleRotary.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <input/SimpleRotaryAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>



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
LiquidCrystal_I2C lcd(_LCD_I2C_ADDRESS, _LCD_COLS, _LCD_ROWS);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, _LCD_COLS, _LCD_ROWS / 2);
LcdMenu menu(renderer);

/* ============================================================================================= */
// BUTTON
/* ============================================================================================= */
ButtonActiveLow buttonStart(_PIN_START, _BUTTON_DEBOUNCE_TIME);
ButtonActiveLow buttonStop(_PIN_STOP, _BUTTON_DEBOUNCE_TIME);

/* ============================================================================================= */
// ENCODER
/* ============================================================================================= */
SimpleRotary encoder(_PIN_ENC_A, _PIN_ENC_B, _PIN_ENC_BUTTON);
SimpleRotaryAdapter rotaryInput(&menu, &encoder);

/* ============================================================================================= */
// TEMPERATURE SENSOR (BME280)
/* ============================================================================================= */
CustomBME280 bme;

/* ============================================================================================= */
// HEATING
/* ============================================================================================= */
GpioOffDelay fan(_PIN_FAN, _FAN_OFFDELAY);

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
//                                      🖥️ LCD MENUS 🖥️
//
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// clang-format off
MENU_SCREEN(mainScreen, mainItems,
    ITEM_INT_RANGE("Heat temp", _TEMP_MIN, _TEMP_MAX, _TEMP_PRESET, callbackTargetHeatTemp, "%dC", 1, true),
    ITEM_INT_RANGE("Heat hum", _HUM_MIN, _HUM_MAX, _HUM_PRESET, callbackTargetHeatHum, "%d%%", 1, true),
    ITEM_STRING_LIST("Heat presets", materialNames, sizeof(materialNames) / sizeof(materialNames[0]), callbackMaterialPreset),
    ITEM_INT_RANGE("Heat time", _TIME_MIN, _TIME_MAX, _TIME_PRESET, callbackTargetHeatTime, "%dh", 1, true),
    ITEM_INT_RANGE("Heat mode", 0, 1, 0, callbackTargetHeatMode, "%d", 1, true),
    ITEM_TOGGLE("Item toggle", callbackToggle),
    ITEM_BASIC("Item Basic"));
// clang-format on

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//
//                                    🔧 SETUP FUNCTIONS 🔧
//
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void createLcdSymbol() {
  // Note: Symbols 0 and 1 are reserved for LcdMenu.

  // Degree Symbol
  static byte degreeSymbol[8] = {
    0b00111,
    0b00101,
    0b00111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcdAdapter.createChar(2, degreeSymbol);
  delay(_SETUP_DELAY);

  // Temperature Symbol (Thermometer)
  static byte tempSymbol[8] = {
    0b00100,
    0b01010,
    0b01010,
    0b01010,
    0b01110,
    0b11111,
    0b01110,
    0b00000
  };
  lcdAdapter.createChar(3, tempSymbol);
  delay(_SETUP_DELAY);

  // Humidity Symbol (Droplet)
  static byte humiditySymbol[8] = {
    0b00100,
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b00000,
    0b00000
  };
  lcdAdapter.createChar(4, humiditySymbol);
  delay(_SETUP_DELAY);

  // Time Symbol (Clock)
  static byte timeSymbol[8] = {
    0b01110,
    0b10001,
    0b10101,
    0b10101,
    0b10001,
    0b01110,
    0b00000,
    0b00000
  };
  lcdAdapter.createChar(5, timeSymbol);
  delay(_SETUP_DELAY);

  // Mode Symbol (Alternative Design)
  static byte modeSymbol[8] = {
    0b00100,
    0b01010,
    0b11111,
    0b01110,
    0b01110,
    0b11111,
    0b01010,
    0b00100
  };
  lcdAdapter.createChar(6, modeSymbol);
  delay(_SETUP_DELAY);

  // Free Slot Placeholder (Empty)
  static byte freeSymbol[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  lcdAdapter.createChar(7, freeSymbol);
  delay(_SETUP_DELAY);
}

void setupLcd() {
  // Copy material names to the array
  for (size_t i = 0; i < _MATERIAL_COUNT; ++i) {
    materialNames[i] = materialPresets[i].name;
  }
  renderer.begin();
  menu.setScreen(mainScreen);
  delay(_SETUP_DELAY);

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
  ledcAttach(_PIN_HEATING, _PWM_FREQUENCY, _PWM_RESOLUTION);

  pidHeating.SetOutputLimits(0, _PWM_MAX_VALUE);
  pidHeating.SetSetpoint(_TEMP_PRESET);
  pidHeating.SetSampleTime(150);
  //pidHeating.SetProportionalMode(0.0);  // Enable proportional on measurement
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
  lcdAdapter.setCursor(0, 2);
  lcdAdapter.draw(byte(3));  // Temperature Symbol
  lcdAdapter.draw("   /   C");
  lcdAdapter.draw(byte(2));  // Degree Symbol

  // Hum actuel / target
  lcdAdapter.setCursor(13, 2);
  lcdAdapter.draw(byte(4));  // Humidity Symbol
  lcdAdapter.draw("  /  %");

  // Heating mode
  lcdAdapter.setCursor(0, 3);
  lcdAdapter.draw(byte(6));  // Mode Symbol
  lcdAdapter.draw("TEMPERATUR");

  // Countdown time actual
  lcdAdapter.setCursor(13, 3);
  lcdAdapter.draw(byte(5));  // Time Symbol
  lcdAdapter.draw("  :  s");
}

void updateHomeContent() {
  // Temp actual
  lcdAdapter.setCursor(1, 2);
  lcd.printf("%3d", actualHeatingValue.temperature);

  // Temp target
  lcdAdapter.setCursor(5, 2);
  lcd.printf("%3d", targetHeatingValue.temperature);

  // Hum actual
  lcdAdapter.setCursor(14, 2);
  lcd.printf("%2d", actualHeatingValue.humidity);

  // Hum target
  lcdAdapter.setCursor(17, 2);
  lcd.printf("%2d", targetHeatingValue.humidity);

  // Countdown heat time actual
  setCountdownHeatTime();
  lcdAdapter.setCursor(14, 3);
  lcd.printf("%2d", actualCountdown.hours);
  lcdAdapter.setCursor(17, 3);
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
    digitalWrite(_PIN_DEBUG_CH0, sensorIsMeasuring);
    if (sensorIsMeasuring == !lastSensorMeasuringState) {

      digitalWrite(_PIN_DEBUG_CH1, HIGH);

      actualHeatingValue.temperature = bme.readTemperature();
      actualHeatingValue.humidity = bme.readHumidity();
      updateHomeContent();

      pidHeating.SetInput(actualHeatingValue.temperature);
      pidHeating.SetInput(map(analogRead(_PIN_DEBUG_POTI), 0, 4095, _TEMP_MIN, _TEMP_MAX));
      pidHeating.SetSetpoint(targetHeatingValue.temperature);
      //Serial.printf("Poti: %d\n", (analogRead(_PIN_DEBUG_POTI)));
      controlHeating();

      digitalWrite(_PIN_DEBUG_CH1, LOW);
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
      digitalWrite(_PIN_DEBUG_CH2, HIGH);
      ledcWrite(_PIN_HEATING, pidHeating.GetOutput());

      Serial.printf("Set:%.2f In:%.2f Out:%.2f\n",
                    pidHeating.GetSetpoint(), pidHeating.GetInput(), mapFloat(pidHeating.GetOutput(), 0.0, 4095.0, 0.0, 100.0));

      digitalWrite(_PIN_DEBUG_CH2, LOW);
    }
  } else {
    ledcWrite(_PIN_HEATING, 0);
    HeatingIsOn = 0;
    pidHeating.SetMode(0);  // 0 = Manual --> Off
  }
  fan.control(HeatingIsOn);
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
  rotaryInput.observe();
  checkHeatSensorStatus();
}