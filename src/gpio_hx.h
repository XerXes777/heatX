/**
 * @file gpio_hx.h
 * @brief GPIO utility classes and functions.
 * @details This file contains classes for managing GPIO functionality:
 *          - **ButtonActiveLow**: Handles button input with active-low logic and debounce handling.
 *          - **GpioOffDelay**: Manages GPIO state with an off-delay mechanism.
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

#ifndef GPIO_HX_H
#define GPIO_HX_H

#include <Arduino.h>

/**
 * @brief Initializes debug pins for output and sets them to LOW.
 * @details Configures GPIO debug pins for use with a logic analyzer.
 */
void setupDebug();

/**
 * @brief Initializes the serial communication interface.
 * @details Configures the serial port with the baud rate `_SERIAL_BAUD` (115200), 
 * the standard default for ESP32.
 */
void setupSerial();

/**
 * @brief Class to handle button input with active-low logic and debounce handling.
 * @details This class provides functionality to handle a button connected to a GPIO pin 
 *          with active-low logic. It includes debouncing to prevent false triggering 
 *          due to mechanical switch bouncing.
 *          
 * ### Example Usage
 * ```cpp
 * #include "gpio_hx.h"
 * 
 * ButtonActiveLow button(2, 50);  // Initialize button on GPIO pin 2 with 50 ms debounce time
 * 
 * void setup() {
 *   Serial.begin(9600);
 * }
 * 
 * void loop() {
 *   button.update();  // Update button state
 * 
 *   if (button.isPressed()) {
 *     Serial.println("Button is pressed!");
 *   } else {
 *     Serial.println("Button is not pressed.");
 *   }
 * 
 *   delay(100);  // Small delay to avoid flooding the Serial monitor
 * }
 * ```
 * @version 1.0.0
 * @date 2024-11-21
 * @author Kevin Hinrichs
 */

class ButtonActiveLow {
private:
  uint8_t pin;             /**< Pin to which the button is connected. */
  uint64_t lastMillis;     /**< Time of the last state change. */
  uint64_t debounceTime;   /**< Debounce time in milliseconds. */
  int32_t lastButtonState; /**< Last state of the button. */
  int32_t buttonState;     /**< Current state of the button. */

public:
  /**
   * @brief Constructor to initialize the button with the pin and debounce time.
   * @param buttonPin The GPIO pin to which the button is connected.
   * @param debounceDuration The debounce time in milliseconds.
   */
  ButtonActiveLow(uint8_t buttonPin, uint64_t debounceDuration)
    : pin(buttonPin),
      debounceTime(debounceDuration),
      lastMillis(0),
      lastButtonState(0),
      buttonState(0) {
    pinMode(pin, INPUT_PULLUP);  // Configure pin as input with pull-up
  }

  /**
   * @brief Updates the button state and handles debouncing.
   * @details This method should be called in a loop to monitor and update the button's 
   *          state. It ensures that the button state changes only after the debounce 
   *          duration has elapsed.
   */
  void update() {
    int readButtonState = digitalRead(pin);  // Read the button state
    unsigned long currentMillis = millis();

    // Check for a state change
    if (readButtonState != lastButtonState) {
      lastMillis = currentMillis;  // Record the time of the state change
    }

    // If debounce time has passed, update the button state
    if (currentMillis - lastMillis >= debounceTime) {
      if (readButtonState != buttonState) {
        buttonState = readButtonState;  // Update the stable state of the button
      }
    }

    lastButtonState = readButtonState;  // Save the last read state
  }

  /**
   * @brief Checks if the button is pressed.
   * @details This method uses active-low logic, meaning it returns true when the button 
   *          state is LOW.
   * @return true if the button is pressed, false otherwise.
   */
  bool isPressed() const {
    return buttonState == LOW;  // Returns true if the button is pressed
  }
};

/**
 * @brief Class to manage GPIO state with off-delay functionality.
 * @details This class allows you to control a GPIO pin, ensuring that it stays on 
 *          for a specified duration (off-delay) before turning off when requested.
 * 
 * ### Example Usage
 * ```cpp
 * GpioOffDelay fanControl(5, 3000);  // Control GPIO pin 5 with a 3-second off delay
 * 
 * void loop() {
 *   fanControl.control(true);   // Turn GPIO on
 *   delay(5000);                // Wait for 5 seconds
 *   fanControl.control(false);  // Request GPIO off
 *   fanControl.update();        // Ensure proper state management
 * }
 * ```
 * @version 1.0.0
 * @date 2024-11-21
 * @author Kevin Hinrichs
 */
class GpioOffDelay {
private:
  unsigned long offStartTime = 0;   /**< Stores the start time of the off delay. */
  bool isOffDelayActive = false;    /**< Indicates if the off delay is active. */
  bool powerOn = false;             /**< Desired state of the GPIO pin. */
  const uint8_t pin;                /**< The GPIO pin to control. */
  const unsigned long offDelayTime; /**< The off delay time in milliseconds. */

public:
  /**
   * @brief Constructor to initialize the GPIO pin and off-delay time.
   * @param controlPin The GPIO pin to be controlled.
   * @param delayTime The duration of the off delay in milliseconds.
   */
  GpioOffDelay(uint8_t controlPin, unsigned long delayTime)
    : pin(controlPin), offDelayTime(delayTime) {
    pinMode(pin, OUTPUT);    // Set the GPIO pin as an output
    digitalWrite(pin, LOW);  // Ensure the pin starts in the OFF state
  }

  /**
   * @brief Updates the state of the GPIO pin based on the off-delay logic.
   * @details This method should be called cyclically to ensure that the off-delay 
   *          mechanism functions correctly. If the GPIO is set to OFF, the method 
   *          keeps the pin ON until the off-delay time has elapsed.
   */
  void update() {
    if (powerOn) {
      digitalWrite(pin, HIGH);   // Turn the GPIO pin on
      isOffDelayActive = false;  // Reset delay flag
    } else {
      if (!isOffDelayActive) {
        offStartTime = millis();  // Save the time when off delay starts
        isOffDelayActive = true;  // Set the delay flag
      }
      // Check if the off delay time has elapsed
      if (isOffDelayActive && (millis() - offStartTime >= offDelayTime)) {
        digitalWrite(pin, LOW);    // Turn the GPIO pin off
        isOffDelayActive = false;  // Reset delay flag
      }
    }
  }

  /**
   * @brief Sets the desired state of the GPIO pin.
   * @param state The desired state of the GPIO (true = ON, false = OFF).
   */
  void control(bool state) {
    powerOn = state;
  }
};


#endif  // GPIO_HX_H