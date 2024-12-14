#define _PIN_DEBUG_CH4 4  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH5 5  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH6 6  ///< GPIO pin for debug with logic analyzer
#define _PIN_DEBUG_CH7 7  ///< GPIO pin for debug with logic analyzer


#include <Wire.h>

#define _LCD_ADDRESS 0x7C >> 1  // 7-Bit-Adresse des AiP31068L

class AIP31068L {
private:
  uint8_t lcdAddress;  // Interne Variable zur Speicherung der Adresse

public:
  AIP31068L(uint8_t address) {
    lcdAddress = address;
  }

  void write() {
    uint8_t controlByte;
    uint8_t dataByte;
    uint8_t error;
    controlByte = 0x00;
    dataByte = 0x00;

    Wire.beginTransmission(lcdAddress);
    Wire.write(controlByte);
    Wire.write(dataByte);
    error = Wire.endTransmission(true);

    if (error != 0) {
      Serial.println("Fehler: Übertragung fehlgeschlagen");
    }
  }

  void read() {
    Wire.requestFrom(lcdAddress, 2, true);
  }

  void send() {
    uint8_t controlByte;
    uint8_t dataByte;
    uint8_t error;
    controlByte = 0x00;
    dataByte = 0x00;

    // Wire.beginTransmission(lcdAddress);
    Wire.requestFrom(lcdAddress, 1, false);
    Wire.write(controlByte);
    Wire.write(dataByte);
    error = Wire.endTransmission(true);


    if (error != 0) {
      Serial.println("Fehler: Übertragung fehlgeschlagen");
    }
  }
};

AIP31068L lcd(_LCD_ADDRESS);

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


  digitalWrite(_PIN_DEBUG_CH4, HIGH);
  Wire.begin();
  digitalWrite(_PIN_DEBUG_CH4, LOW);

  digitalWrite(_PIN_DEBUG_CH5, HIGH);
  lcd.send();
  digitalWrite(_PIN_DEBUG_CH5, LOW);

  // digitalWrite(_PIN_DEBUG_CH6, HIGH);
  // lcd.read();
  // digitalWrite(_PIN_DEBUG_CH6, LOW);
}

void loop() {
}