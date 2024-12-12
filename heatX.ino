#include <Wire.h>

class AIP31068L {
private:
  const uint8_t lcdAddress = 0x7C;  // I2C-Adresse des AiP31068L

public:
  // Initialisierung des I2C-Busses
  void begin() {
    Wire.begin();
  }

  // Methode, um ein Register zu lesen
  uint8_t readRegister(uint8_t regAddress) {
    uint8_t data = 0;

    Wire.beginTransmission(lcdAddress);
    Wire.write(regAddress);  // Registeradresse senden
    if (Wire.endTransmission(false) != 0) {
      Serial.println("Fehler: LCD-Gerät nicht erreichbar");
      return 0xFF;  // Fehlercode
    }

    Wire.requestFrom(lcdAddress, (uint8_t)1);  // Lese 1 Byte
    if (Wire.available()) {
      data = Wire.read();
    }

    return data;
  }

  // Testmethode zum Debuggen
  void testReadRegister(uint8_t regAddress) {
    uint8_t value = readRegister(regAddress);
    if (value != 0xFF) {
      Serial.print("Register 0x");
      Serial.print(regAddress, HEX);
      Serial.print(" hat den Wert: 0x");
      Serial.println(value, HEX);
    }
  }
};

AIP31068L lcd;

void setup() {
  Serial.begin(115200);
  lcd.begin();

  // Beispiel: Lese ein bekanntes Register (z.B. Busy Flag oder Statusregister)
  lcd.testReadRegister(0x00);  // Setze hier die Registeradresse ein
}

void loop() {
  // Kein Loop-Code erforderlich
}
