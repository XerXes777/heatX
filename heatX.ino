#define _PIN_DEBUG_CH4 4  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH5 5  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH6 6  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH7 7  ///< GPIO pin for debug with logic analyzer

#define _LCD_ADDRESS 0x7C  // 7-Bit-Adresse des AiP31068L

#include <Wire.h>
#include <esp32-hal-i2c.h>

// LCD-Konfiguration
#define I2C_PORT_NUM 0         // I2C-Port (Nummer 0 oder 1)
#define LCD_ADDRESS 0x7C >> 1  // 7-Bit-Adresse des LCD

bool readBusyFlagAndAddress(uint8_t &addressCounter) {
  esp_err_t ret;
  uint8_t status = 0;

  // 1. Sende leeres Byte (0x00) an das LCD
  uint8_t emptyByte = 0x00;
  ret = i2cWrite(I2C_PORT_NUM, LCD_ADDRESS, &emptyByte, 1, 1000);
  if (ret != ESP_OK) {
    Serial.println("Fehler: Leeres Byte konnte nicht gesendet werden.");
    return false;
  }

  // 2. Lese das Statusbyte (Busy-Flag und Address Counter)
  ret = i2cRead(I2C_PORT_NUM, LCD_ADDRESS, &status, 1, 1000, nullptr);
  if (ret != ESP_OK) {
    Serial.println("Fehler: Statusbyte konnte nicht gelesen werden.");
    return false;
  }

  // 3. Status auswerten
  addressCounter = status & 0x7F;  // Address Counter (untere 7 Bits)
  return (status & 0x80) != 0;     // Busy-Flag (MSB)
}



void testBusyFlagAndAddress() {
  uint8_t addressCounter = 0;
  bool isBusy = false;
  // bool isBusy = readBusyFlagAndAddress(addressCounter);

  if (isBusy) {
    Serial.println("LCD ist beschäftigt (Busy-Flag gesetzt).");
  } else {
    Serial.println("LCD ist bereit (Busy-Flag nicht gesetzt).");
  }

  Serial.print("Address Counter: 0x");
  Serial.println(addressCounter, HEX);
}



void setup() {
  Serial.begin(115200);

  pinMode(_PIN_DEBUG_CH4, OUTPUT);
  pinMode(_PIN_DEBUG_CH5, OUTPUT);
  pinMode(_PIN_DEBUG_CH6, OUTPUT);
  pinMode(_PIN_DEBUG_CH7, OUTPUT);

  digitalWrite(_PIN_DEBUG_CH4, LOW);
  digitalWrite(_PIN_DEBUG_CH5, LOW);
  digitalWrite(_PIN_DEBUG_CH6, LOW);
  digitalWrite(_PIN_DEBUG_CH7, LOW);

  // I2C-Bus initialisieren
  Wire.begin();

  // Teste das Busy-Flag und den Address Counter
  testBusyFlagAndAddress();
}


void loop() {
}