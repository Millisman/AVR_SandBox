#ifndef LIQUIDCRYSTAL_H_INCLUDED
#define LIQUIDCRYSTAL_H_INCLUDED

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>


// commands
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT      0x00
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTRYSHIFTINCREMENT     0x01
#define LCD_ENTRYSHIFTDECREMENT     0x00

// flags for display on/off control
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE     0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

// flags for function set
#define LCD_4BITMODE    0x00
#define LCD_2LINE       0x08
#define LCD_1LINE       0x00
#define LCD_5x10DOTS    0x04
#define LCD_5x8DOTS     0x00


// namespace app {
//
// class LiquidCrystal {
// public:
//     static void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
//     static void clear();
//     static void home();
//     static void noDisplay();
//     static void display();
//     static void noBlink();
//     static void blink();
//     static void noCursor();
//     static void cursor();
//     static void scrollDisplayLeft();
//     static void scrollDisplayRight();
//     static void leftToRight();
//     static void rightToLeft();
//     static void autoscroll();
//     static void noAutoscroll();
//     static void setRowOffsets(int row1, int row2, int row3, int row4);
//     static void createChar(uint8_t, uint8_t[]);
//     static void setCursor(uint8_t, uint8_t);
//     static uint8_t write(char value);
//     static void command(uint8_t value);
// private:
//     static void send(uint8_t, uint8_t);
//     static void write4bits(uint8_t);
//     static void pulseEnable();
//
// };
//
// }

// #define LCD_RS  D, 6, H
// #define LCD_EN  D, 7, H
// #define LCD_D4  D, 2, H
// #define LCD_D5  D, 3, H
// #define LCD_D6  D, 4, H
// #define LCD_D7  D, 5, H






void LiquidCrystal_begin(uint8_t cols, uint8_t lines, uint8_t dotsize);
void LiquidCrystal_setRowOffsets(int row0, int row1, int row2, int row3);
void LiquidCrystal_clear();
void LiquidCrystal_home();
void LiquidCrystal_setCursor(uint8_t col, uint8_t row);
void LiquidCrystal_noDisplay();
void LiquidCrystal_display();
void LiquidCrystal_noCursor();
void LiquidCrystal_cursor();
void LiquidCrystal_noBlink();
void LiquidCrystal_blink();
void LiquidCrystal_scrollDisplayLeft(void);
void LiquidCrystal_scrollDisplayRight(void);
void LiquidCrystal_leftToRight(void);
void LiquidCrystal_rightToLeft(void);
void LiquidCrystal_autoscroll(void);
void LiquidCrystal_noAutoscroll(void);
void LiquidCrystal_createChar(uint8_t location, uint8_t charmap[]);
void LiquidCrystal_command(uint8_t value);
uint8_t LiquidCrystal_write(char value);
void LiquidCrystal_send(uint8_t value, uint8_t mode);
void LiquidCrystal_pulseEnable(void);
void LiquidCrystal_write4bits(uint8_t value);

#endif // LIQUIDCRYSTAL_H_INCLUDED
