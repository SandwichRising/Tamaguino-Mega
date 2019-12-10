/* =====================
 *
 * Board Settings
 *
 * New boards (pin layouts) and screen modes get added here
 * Makes it easy to swap between what you're uploading to by 
 * changing #defines at the top of the main .ino
 *
 ====================== */

#include <Arduino.h>

//using internal pullup resistors or external pulldown
#ifdef BUTTON_RESISTORS_INTERNAL
  #define ACTIVATED LOW
#endif
#ifdef BUTTON_RESISTORS_EXTERNAL
  #define ACTIVATED HIGH
#endif

// PIN SETTINGS
#ifdef BOARD_TYPE_ATMEGA1284
  const int button1Pin = 12;
  const int button2Pin = 13; 
  const int button3Pin = 14; 
  const int ledPin = 9;
  const int soundPin = 10; 
  //to read battery charge
  #define VOLTAGE_PIN A4
  //screen SPI pins are below for this board
#endif

// SCREEN SETTINGS
#ifdef SCREEN_DRIVER_1106
  #include <Adafruit_SH1106.h>
#endif

#ifdef SCREEN_TYPE_SPI
  #ifdef BOARD_TYPE_ATMEGA1284
    // FOR 4 PIN SPI 
    #define OLED_MOSI   5
    #define OLED_CLK   7
    #define OLED_DC    3
    #define OLED_CS    4
    #define OLED_RESET 2
    Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
  #endif
#endif
#ifdef SCREEN_TYPE_I2C
  #ifdef SCREEN_DRIVER_1306
    Adafruit_SSD1306 display(-1);
  #endif
  #ifdef SCREEN_DRIVER_1106
    Adafruit_SH1106 display(-1);
  #endif
#endif
