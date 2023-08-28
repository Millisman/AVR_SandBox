#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <avr/wdt.h>
#include "lcd/lcd.h"
#include "lcd/LiquidCrystal.h"
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "quadrature/quadrature.h"
#include "avr/pin_macros.h"
#include "avr/uart.h"

int putc_uart0(char ch, FILE *stream) {
    (void)stream;
    if (ch == '\n') { uart0_putc('\r'); }
    uart0_putc(ch);
    return 0;
}


volatile uint8_t _t0_tick;
void init_timer0() {
    // 250hz
    // http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
    cli();
    TCCR0 = 0; // set entire TCCR0A register to 0
    TCCR0 = 0; // same for TCCR0B
    TCNT0  = 0; // initialize counter value to 0
    // set compare match register for 250 Hz increments
    OCR0 = 124; // = 8000000 / (256 * 250) - 1 (must be <256)
    // turn on CTC mode
    TCCR0 |= (1 << WGM01);
    // Set CS02, CS01 and CS00 bits for 256 prescaler
    TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00);
    // enable timer compare interrupt
    TIMSK |= (1 << OCIE0);
    sei();
}

#define INPUT_A  C, 0
#define INPUT_B  C, 1
#define INPUT_C  D, 7

typedef enum {
    ev_none,
    ev_encoder_turn,
    ev_encoder_right,
    ev_encoder_left,
    ev_encoder_pressed,
    ev_encoder_released,
    ev_1sec_tick
} input_event_t;

volatile  uint8_t __events_global;
volatile     bool __encoder_button_new,  __encoder_button_old, __encoder_reset;
volatile  uint8_t __encoder_state_new,   __encoder_state_old;

volatile  uint16_t __tim_btn_tick;


ISR(TIMER0_COMP_vect) {

    if (_t0_tick > 249) { // 250 Hz
        _t0_tick = 0;
        __events_global |= _BV(ev_1sec_tick);
    } else {
        _t0_tick++;
    }
    __tim_btn_tick++;
    __encoder_button_new =  PIN_L(INPUT_C); //       LATCH_L(INPUT_C);

    if (__encoder_button_new != __encoder_button_old) {

        if (__encoder_button_new) {
            __events_global |= _BV(ev_encoder_pressed);
        } else {
            __events_global |= _BV(ev_encoder_released);
        }

        __encoder_button_old = __encoder_button_new;
    }

    __encoder_state_new = (PIN_L(INPUT_A) << 0) | (PIN_L(INPUT_B) << 1);

    if (__encoder_reset && __encoder_state_new == 0b11) {

        if (__encoder_state_old == 0b10) {
            __events_global |= _BV(ev_encoder_left);
        }

        if (__encoder_state_old == 0b01) {
            __events_global |= _BV(ev_encoder_right);
        }

        __encoder_reset = false;
        __events_global |= _BV(ev_encoder_turn);
    }

    if (__encoder_state_new == 0b00) {
        __encoder_reset = true;
    }
    __encoder_state_old = __encoder_state_new;
}

void encoder_init() {
    // inputs
    DRIVER(INPUT_A, PULLUP);
    DRIVER(INPUT_B, PULLUP);
    DRIVER(INPUT_C, PULLUP); // enc_button
}

volatile uint8_t _info_tick;
void about_code() {
    _info_tick++;
    LiquidCrystal_noCursor();
    LiquidCrystal_noBlink();
    LiquidCrystal_setCursor(0,3);
    if (_info_tick < 5) {
        fprintf_P(stderr, PSTR("\4Written by Sergey A.\n"));
    } else if (_info_tick < 10) {
        fprintf_P(stderr, PSTR("\4Kostyanoy @ 2023 USA\n"));
    } else if (_info_tick < 20) {
        fprintf_P(stderr, PSTR("\4 contact@arduino.uno\n"));
    } else {
        time_t t = time(NULL);
        struct tm* ptr = localtime(&t);
        fprintf_P(stderr, PSTR("\4%s\n"), isotime(ptr));
    }

}


volatile FILE file_uart0;

int main() {
    MCUCSR &= ~(1 << WDRF);
    wdt_disable();
    wdt_enable(WDTO_500MS);

    __events_global = 0;
    __encoder_button_new = false;
    __encoder_button_old = false;
    __encoder_reset = false;

    uart_init(UART_BAUD_SELECT(250000, F_CPU));
    sei();

    fdev_setup_stream( &file_uart0, putc_uart0, NULL, _FDEV_SETUP_WRITE);

    stdout = &file_uart0;
    printf_P(PSTR("UART init at 250000\n"));

    printf_P(PSTR("LCD init\n"));
    init_lcd();

    printf_P(PSTR("Tick (encoder driver) init\n"));
    encoder_init();
    init_timer0();

    printf_P(PSTR("Quadrature T1 init\n"));
    quadrature_init();

    set_system_time(1692421501 - UNIX_OFFSET);

    while (1) {

        if(__events_global & _BV(ev_1sec_tick)) {
            __events_global &= ~_BV(ev_1sec_tick);
            system_tick(); // sec++
            about_code();
            quadrature_freq_lcd();
        }

        if(__events_global & _BV(ev_encoder_turn)) {
            __events_global &= ~_BV(ev_encoder_turn);
            // Enable Backlight
        }

        if(__events_global & _BV(ev_encoder_right)) {
            __events_global &= ~_BV(ev_encoder_right);
            quadrature_encoder_right();
        }

        if(__events_global & _BV(ev_encoder_left)) {
            __events_global &= ~_BV(ev_encoder_left);
            quadrature_encoder_left();
        }

        if(__events_global & _BV(ev_encoder_pressed)) {
            __events_global &= ~_BV(ev_encoder_pressed);
            __tim_btn_tick = 0;
        }

        if(__events_global & _BV(ev_encoder_released)) {
            __events_global &= ~_BV(ev_encoder_released);
            if (__tim_btn_tick > 249) {
                quadrature_encoder_released_long();
            } else {
                quadrature_encoder_released_short();
            }
        }

        wdt_reset();
    }
}




