#include "lcd.h"
#include "LiquidCrystal.h"

static FILE file_lcd0;


#define LCD_WIDTH 20
#define LCD_HEIGHT 4

static uint8_t _lcd_carriage;
static uint8_t _lcd_line;

int putc_lcd0(char ch, FILE *stream) {
    (void)stream;
    if ( ch >= ' ' && ch <= '~') {
        LiquidCrystal_write(ch);
        _lcd_carriage++;
    } else {
        if ( (uint8_t)ch >= '\1' && (uint8_t)ch <= '\4') {
            _lcd_line = (uint8_t)ch - 1;
            _lcd_carriage = 0;
            LiquidCrystal_setCursor(_lcd_carriage, _lcd_line);
        } else if ( ch == '\n' ) {
            for (uint8_t i = _lcd_carriage; i < LCD_WIDTH; i++) {
                LiquidCrystal_write(' ');
            }
        }
    }
    return 0;
}

void lcd_col(uint8_t col) {
    LiquidCrystal_setCursor(col, _lcd_line);
}


void init_lcd() {
    _lcd_carriage = 0;
    _lcd_line = 0;
    LiquidCrystal_begin(LCD_WIDTH, LCD_HEIGHT, LCD_5x8DOTS);
    fdev_setup_stream( &file_lcd0, putc_lcd0,   NULL, _FDEV_SETUP_WRITE);
    stderr = &file_lcd0;
}




