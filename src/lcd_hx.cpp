/**
 * @file lcd_hx.cpp
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
 * All rights reserved. Unauthorized copying or use of this code is prohibited.
 */

#include "lcd_hx.h"
#include "globals_hx.h"

/* LcdMenu API:
class DisplayInterface:

virtual void begin() = 0;
virtual void clear() = 0;
virtual void show() = 0;
virtual void hide() = 0;
virtual void draw(uint8_t byte) = 0;
virtual void draw(const char* text) = 0;
virtual void setCursor(uint8_t col, uint8_t row) = 0;
virtual void setBacklight(bool enabled) = 0;


class CharacterDisplayInterface:

virtual void createChar(uint8_t id, uint8_t* c) = 0;
virtual void drawBlinker() = 0;
virtual void clearBlinker() = 0;


class LiquidCrystal_I2CAdapter:


void begin() override {
  lcd->init();
  lcd->clear();
  lcd->backlight();
}

void createChar(uint8_t id, uint8_t* c) override {
  lcd->createChar(id, c);
}

void setBacklight(bool enabled) override {
  lcd->setBacklight(enabled);
}

void setCursor(uint8_t col, uint8_t row) override {
  lcd->setCursor(col, row);
}

void draw(const char* text) override {
  lcd->print(text);
}

void draw(uint8_t byte) override {
  lcd->write(byte);
}

void drawBlinker() override {
  lcd->blink();
}

void clearBlinker() override {
  lcd->noBlink();
}

void show() override {
  lcd->display();
  lcd->backlight();
}

void hide() override {
  lcd->noDisplay();
  lcd->noBacklight();
}

void clear() override {
  lcd->clear();
}

*/
