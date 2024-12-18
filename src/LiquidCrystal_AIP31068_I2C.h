// YWROBOT
#ifndef LiquidCrystal_AIP31068_I2C_h
#define LiquidCrystal_AIP31068_I2C_h

#include <inttypes.h>
#include <Print.h>
#include <Wire.h>

#define RGB_ADDRESS (0xc0 >> 1)

/*!
 *  color define
 */
#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3

#define REG_RED 0x04    // pwm2
#define REG_GREEN 0x03  // pwm1
#define REG_BLUE 0x02   // pwm0

#define REG_MODE1 0x00
#define REG_MODE2 0x01
#define REG_OUTPUT 0x08

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define En 0b00000000  // Enable bit
#define Rw 0b00000000  // Read/Write bit
#define Rs 0b01000000  // Register select bit

class LiquidCrystal_AIP31068_I2C : public Print {
public:
  LiquidCrystal_AIP31068_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows);
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  void clear();
  void home();
  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void printLeft();
  void printRight();
  void leftToRight();
  void rightToLeft();
  void shiftIncrement();
  void shiftDecrement();
  void autoscroll();
  void noAutoscroll();
  void createChar(uint8_t, uint8_t[]);
  void createChar(uint8_t location, const char *charmap);
  void setReg(uint8_t addr, uint8_t data);
  void setRGB(uint8_t r, uint8_t g, uint8_t b);
  void setColorWhite() {
    setRGB(255, 255, 255);
  }
  // Example: 	const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};

  void setCursor(uint8_t, uint8_t);
#if defined(ARDUINO) && ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void write(uint8_t);
#endif
  void command(uint8_t);
  void init();
  void oled_init();

  ////compatibility API function aliases
  void blink_on();                                              // alias for blink()
  void blink_off();                                             // alias for noBlink()
  void cursor_on();                                             // alias for cursor()
  void cursor_off();                                            // alias for noCursor()
  void load_custom_character(uint8_t char_num, uint8_t *rows);  // alias for createChar()
  void printstr(const char[]);

  ////Unsupported API functions (not implemented in this library)
  uint8_t status();
  void setContrast(uint8_t new_val);
  uint8_t keypad();
  void setDelay(int, int);
  void on();
  void off();
  uint8_t init_bargraph(uint8_t graphtype);
  void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end);
  void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len, uint8_t pixel_col_end);


private:
  void init_priv();
  void send(uint8_t, uint8_t);
  void write4bits(uint16_t);
  void write8bits(uint16_t);
  void controllerWrite(uint16_t);
  //  void pulseEnable(uint8_t);
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  bool _oled = false;
  uint8_t _cols;
  uint8_t _rows;
};

#endif
