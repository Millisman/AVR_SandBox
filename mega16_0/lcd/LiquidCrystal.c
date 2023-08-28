#include "LiquidCrystal.h"
#include "LiquidCrystalPins.h"
// // #include <avr/io.h>
#include <stdio.h>
#include <string.h>
// // // // // // // #include <inttypes.h>
#include <util/delay_basic.h>
#include <util/delay.h>

#include "../avr/pin_macros.h"


// namespace app {
    
static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
static uint8_t _numlines;
static uint8_t _numcols;
static uint8_t _row_offsets[4];

void LiquidCrystal_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    DRIVER(LCD_RS, OUT);
    DRIVER(LCD_EN, OUT);
    DRIVER(LCD_D4, OUT);
    DRIVER(LCD_D5, OUT);
    DRIVER(LCD_D6, OUT);
    DRIVER(LCD_D7, OUT);
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    
    if (lines > 1) _displayfunction |= LCD_2LINE;
    _numcols = cols;
    _numlines = lines;

    LiquidCrystal_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);
    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) _displayfunction |= LCD_5x10DOTS;
    _delay_ms(50);
    CLR(LCD_RS);
    CLR(LCD_EN);
    
    LiquidCrystal_write4bits(0x03); // set 4 bit mode
    _delay_ms(5); // wait min 4.1ms
    LiquidCrystal_write4bits(0x03); // set 4 bit mode
    _delay_ms(5); // wait min 4.1ms
    LiquidCrystal_write4bits(0x03); // set 4 bit mode
    _delay_us(150);
    LiquidCrystal_write4bits(0x02);
    LiquidCrystal_command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LiquidCrystal_display();
    LiquidCrystal_clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    LiquidCrystal_command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal_setRowOffsets(int row0, int row1, int row2, int row3) {
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
}

void LiquidCrystal_clear() {
    LiquidCrystal_command(LCD_CLEARDISPLAY);
    _delay_ms(2);
}

void LiquidCrystal_home() {
    LiquidCrystal_command(LCD_RETURNHOME);
    _delay_ms(2);
}

void LiquidCrystal_setCursor(uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if ( row >= max_lines ) { row = max_lines - 1; }
    if ( row >= _numlines ) { row = _numlines - 1; }
    LiquidCrystal_command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

void LiquidCrystal_noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_display() {
    _displaycontrol |= LCD_DISPLAYON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_cursor() {
    _displaycontrol |= LCD_CURSORON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_blink() {
    _displaycontrol |= LCD_BLINKON;
    LiquidCrystal_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_scrollDisplayLeft(void) {
    LiquidCrystal_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal_scrollDisplayRight(void) {
    LiquidCrystal_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LiquidCrystal_leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    LiquidCrystal_command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal_rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    LiquidCrystal_command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal_autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    LiquidCrystal_command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal_noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    LiquidCrystal_command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal_createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    LiquidCrystal_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i=0; i<8; i++) {
        LiquidCrystal_write(charmap[i]);
    }
}

void   LiquidCrystal_command(uint8_t value) { LiquidCrystal_send(value, 0); }
uint8_t LiquidCrystal_write(char value) { LiquidCrystal_send(value, 1); return 1; } //!

void LiquidCrystal_send(uint8_t value, uint8_t mode) {
    if (mode) { SET(LCD_RS); } else { CLR(LCD_RS); }
    LiquidCrystal_write4bits(value >> 4);
    LiquidCrystal_write4bits(value);
}

void LiquidCrystal_pulseEnable(void) {
    CLR(LCD_EN);  _delay_us(0.6);
    SET(LCD_EN);   _delay_us(0.6);
    CLR(LCD_EN);  _delay_us(40);
}

void LiquidCrystal_write4bits(uint8_t value) {
    if ((value >> 0) & 0x01) { SET(LCD_D4); } else { CLR(LCD_D4); }
    if ((value >> 1) & 0x01) { SET(LCD_D5); } else { CLR(LCD_D5); }
    if ((value >> 2) & 0x01) { SET(LCD_D6); } else { CLR(LCD_D6); }
    if ((value >> 3) & 0x01) { SET(LCD_D7); } else { CLR(LCD_D7); }
    LiquidCrystal_pulseEnable();
}

// }
