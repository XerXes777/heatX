/**
 * @file pid_hx.h
 * @brief PID controller implementation for precise control systems.
 * @details This file contains the implementation of a flexible and configurable 
 *          PID controller (`PID_heatX`) for feedback-based systems.
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
 * All rights reserved. Unauthorized copying or use of this code is prohibited.
 */

#ifndef PID_HX_H
#define PID_HX_H

#include <Arduino.h>

/**
 * @brief PID controller class for controlling systems based on feedback.
 * @details This class implements a PID (Proportional-Integral-Derivative) controller with:
 *          - Support for proportional on error or measurement.
 *          - Adjustable sample time, output limits, and tuning parameters.
 *          - Automatic or manual modes.
 *
 * ### Example Usage
 * ```cpp
 * #include "pid_hx.h"
 * 
 * PID_heatX temperaturePID(2.0, 5.0, 1.0, 0);  // Kp=2.0, Ki=5.0, Kd=1.0, DIRECT control
 * 
 * void setup() {
 *   Serial.begin(9600);
 *   temperaturePID.SetOutputLimits(0, 255);  // Set output range
 *   temperaturePID.SetSampleTime(100);       // Set sample time to 100 ms
 *   temperaturePID.SetMode(true);            // Enable automatic mode
 * }
 * 
 * void loop() {
 *   float currentTemperature = ReadTemperatureSensor();  // Your function to get input
 *   temperaturePID.SetInput(currentTemperature);         // Update input
 *   temperaturePID.SetSetpoint(25.0);                    // Desired temperature
 * 
 *   if (temperaturePID.Compute()) {                      // Perform PID computation
 *     float controlOutput = temperaturePID.GetOutput();  // Get output
 *     ApplyHeaterPower(controlOutput);                   // Apply output to heater
 *   }
 * }
 * ```
 */
class PID_heatX {
private:
  unsigned long lastTime;        /**< Timestamp of the last computation. */
  float input, output, setpoint; /**< Process variable, output, and setpoint values. */
  float outputSum, lastInput;    /**< Integral term and previous process variable value. */
  float kp, ki, kd;              /**< PID tuning parameters. */
  int sampleTime;                /**< Sample time in milliseconds. */
  float outMin, outMax;          /**< Minimum and maximum output limits. */
  bool inAuto;                   /**< Flag indicating if the controller is in automatic mode. */
  bool pOnE, pOnM;               /**< Flags for proportional on error and measurement. */
  float pOnEKp, pOnMKp;          /**< Scaled proportional constants. */
  int controllerDirection;       /**< Controller direction: DIRECT (0) or REVERSE (1). */

public:
  /**
   * @brief Constructor: Initializes the PID controller.
   * @param Kp Proportional gain.
   * @param Ki Integral gain.
   * @param Kd Derivative gain.
   * @param ControllerDirection Control direction: DIRECT (0) or REVERSE (1).
   */
  PID_heatX(float Kp, float Ki, float Kd, int ControllerDirection)
    : kp(Kp), ki(Ki), kd(Kd), sampleTime(100), inAuto(false), lastTime(0),
      outputSum(0), lastInput(0), pOnE(true), pOnM(false),
      controllerDirection(ControllerDirection), setpoint(0),
      input(0), output(0), outMin(0), outMax(255) {
    SetTunings(Kp, Ki, Kd);
    SetProportionalMode(1.0f);
  }

  /**
   * @brief Perform PID computation.
   * @details Computes the PID output based on the current setpoint and input.  
   *          This function should be called frequently, preferably in the loop function.  
   *          New computations are performed only after the specified sample time has elapsed.
   * @return 1 if a new output is computed, 0 otherwise.
   */
  int Compute() {
    if (!inAuto) return 0;  // No computation if not in automatic mode

    unsigned long now = millis();
    int timeChange = now - lastTime;

    if (timeChange >= sampleTime) {
      // Calculate error and derivative
      float error = setpoint - input;
      float dInput = input - lastInput;

      // Update the integral term
      outputSum += (ki * error);

      // Adjust for proportional on measurement (if enabled)
      if (pOnM) outputSum -= pOnMKp * dInput;

      // Clamp the output sum to prevent windup
      if (outputSum > outMax) outputSum = outMax;
      else if (outputSum < outMin) outputSum = outMin;

      // Compute the final PID output
      float proportional = pOnE ? (pOnEKp * error) : 0;
      float derivative = -kd * dInput;

      output = proportional + outputSum + derivative;

      // Clamp the output to specified limits
      if (output > outMax) output = outMax;
      else if (output < outMin) output = outMin;

      // Save state for the next iteration
      lastInput = input;
      lastTime = now;

      return 1;  // Indicate that new values were computed
    }

    return 0;  // No new computation
  }

  /**
   * @brief Sets the PID parameters (Kp, Ki, Kd) and adjusts them for the sample time.
   * @param Kp Proportional gain.
   * @param Ki Integral gain.
   * @param Kd Derivative gain.
   */
  void SetTunings(float Kp, float Ki, float Kd) {
    if (Kp < 0 || Ki < 0 || Kd < 0) return;

    float sampleTimeInSec = ((float)sampleTime) / 1000.0f;
    kp = Kp;
    ki = Ki * sampleTimeInSec;
    kd = Kd / sampleTimeInSec;

    // Adjust parameters for reverse direction
    if (controllerDirection == 1) {  // 1 = reverse
      kp = -kp;
      ki = -ki;
      kd = -kd;
    }

    // Update proportional gains based on current proportional mode
    pOnEKp = pOnE ? kp : 0.0f;
    pOnMKp = pOnM ? kp : 0.0f;
  }

  /**
   * @brief Sets the proportional mode (error, measurement, or a mix).
   * @param pOn Proportional mode scaling: 1.0 for full error, 0.0 for full measurement.
   */
  void SetProportionalMode(float pOn) {
    if (pOn < 0 || pOn > 1) return;

    pOnE = (pOn > 0);  // Enable proportional on error if pOn > 0
    pOnM = (pOn < 1);  // Enable proportional on measurement if pOn < 1

    // Update proportional scaling
    pOnEKp = pOn * kp;
    pOnMKp = (1 - pOn) * kp;
  }

  /**
   * @brief Sets the sample time for the PID controller.
   * @param newSampleTime Sample time in milliseconds.
   */
  void SetSampleTime(int newSampleTime) {
    if (newSampleTime > 0) {
      float ratio = (float)newSampleTime / (float)sampleTime;
      ki *= ratio;
      kd /= ratio;
      sampleTime = newSampleTime;
    }
  }

  /**
   * @brief Sets the output limits for the PID controller.
   * @param min Minimum output value.
   * @param max Maximum output value.
   */
  void SetOutputLimits(float min, float max) {
    if (min >= max) return;
    outMin = min;
    outMax = max;

    if (output > outMax) output = outMax;
    else if (output < outMin) output = outMin;

    if (outputSum > outMax) outputSum = outMax;
    else if (outputSum < outMin) outputSum = outMin;
  }

  /**
   * @brief Enables or disables automatic mode.
   * @param mode true to enable automatic mode, false for manual mode.
   */
  void SetMode(bool mode) {
    bool newAuto = mode == 1;  // 1 = Automatic --> On
    if (newAuto != inAuto) {
      Initialize();
    }
    inAuto = newAuto;
  }

  /**
   * @brief Initializes the PID controller state.
   */
  void Initialize() {
    outputSum = output;
    if (outputSum > outMax) outputSum = outMax;
    else if (outputSum < outMin) outputSum = outMin;

    lastInput = input;
  }

  /**
   * @brief Sets the control direction.
   * @param direction Control direction: 0 for DIRECT, 1 for REVERSE.
   */
  void SetControllerDirection(int direction) {
    controllerDirection = direction;
  }

  /**
   * @brief Sets the process variable input.
   * @param newInput Current process variable value.
   */
  void SetInput(float newInput) {
    input = newInput;
  }

  /**
   * @brief Sets the setpoint.
   * @param newSetpoint Desired setpoint value.
   */
  void SetSetpoint(float newSetpoint) {
    setpoint = newSetpoint;
  }

  /**
   * @brief Gets the current input value.
   * @return Current process variable value.
   */
  float GetInput() const {
    return input;
  }

  /**
   * @brief Gets the current output value.
   * @return Current output value.
   */
  float GetOutput() const {
    return output;
  }

  /**
   * @brief Gets the current setpoint value.
   * @return Current setpoint value.
   */
  float GetSetpoint() const {
    return setpoint;
  }
};


#endif  // PID_HX_H