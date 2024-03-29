#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "quadrature.h"
#include "avr/pin_macros.h"
#include <stdbool.h>
#include <stdio.h>
#include "lcd/lcd.h"
#include "lcd/LiquidCrystal.h"

#define TIM_A  D, 5
#define TIM_B  D, 4

void Calc_Digits_Value(digits_value_t *v) {
    v->VALUE = (uint32_t)v->tenthous * 10000 +
    (uint32_t)v->thous * 1000 +
    (uint32_t)v->hundreds * 100 +
    (uint32_t)v->dozens * 10 +
    (uint32_t)v->units;
}





static mod_freq_5num cur_num_freq;
static digits_value_t freq;
static bool _lcd_jump;

void quadrature_init() {
    cur_num_freq = Number4;
    _lcd_jump = false;
    DRIVER(TIM_A, OUT);
    DRIVER(TIM_B, OUT);
}


void quadrature_freq_lcd() {
    Calc_Digits_Value(&freq);
    // // // // // uint32_t ofreq = (uint32_t)freq.tenthous * 10000 +
    // // // // //     (uint32_t)freq.thous * 1000 +
    // // // // //     (uint32_t)freq.hundreds * 100 +
    // // // // //     (uint32_t)freq.dozens * 10 +
    // // // // //     (uint32_t)freq.units;

    // printf_P(PSTR("OFREQ: %lu\n"), ofreq),

    LiquidCrystal_cursor();
    LiquidCrystal_blink();
    fprintf_P(stderr, PSTR("\1FREQ: %u%u%u%u%u HZ\n"), freq.tenthous, freq.thous, freq.hundreds, freq.dozens, freq.units);
    lcd_col((uint8_t)cur_num_freq + 6);
}


void quadrature_turnover_lcd() {

}







void quadrature_lcd_inc_val() {
    switch (cur_num_freq) {
        case Number0: if ((freq.tenthous < 9)) { freq.tenthous++; } break;
        case Number1: if ((freq.thous < 9)) { freq.thous++; } break;
        case Number2: if ((freq.hundreds < 9)) { freq.hundreds++; } break;
        case Number3: if ((freq.dozens < 9)) { freq.dozens++; } break;
        case Number4: if ((freq.units < 9)) { freq.units++; } break;
    }
}

void quadrature_lcd_dec_val() {
    switch (cur_num_freq) {
        case Number0: if ((freq.tenthous > 0)) { freq.tenthous--; } break;
        case Number1: if ((freq.thous > 0)) { freq.thous--; } break;
        case Number2: if ((freq.hundreds > 0)) { freq.hundreds--; } break;
        case Number3: if ((freq.dozens > 0)) { freq.dozens--; } break;
        case Number4: if ((freq.units > 0)) { freq.units--; } break;
    }
}




void quadrature_encoder_right() {
    printf_P(PSTR("quadrature_encoder_right\n"));
    quadrature_lcd_inc_val();
    quadrature_freq_lcd();
}

void quadrature_encoder_left() {
    printf_P(PSTR("quadrature_encoder_left\n"));
    quadrature_lcd_dec_val();
    quadrature_freq_lcd();
}

void quadrature_encoder_released_short() {
    printf_P(PSTR("quadrature_encoder_released_short\n"));
    if (cur_num_freq > Number0) { cur_num_freq--; } else { cur_num_freq = Number4; }
    quadrature_freq_lcd();

}

void quadrature_encoder_released_long() {
    printf_P(PSTR("quadrature_encoder_released_long\n"));
    quadrature_freq_lcd();
}








void quadrature_update() {}



// prescaler of 1 will get us 4MHz - 488Hz
// User a higher prescaler for lower frequencies

#define PRESCALER 1
#define PRESCALER_BITS 0x01

// Output phase shifted waveforms on Arduino Pins 9 & 10
// half_period = 1/2 the period of both waveform in clock ticks (16Mhz clock default) note that the larger this number the lower the freq annd the more phase shift resolution you get
// shift = number of ticks delay between leading waveform and trailing. Must be < half_period
// invert_b- should we initially invert B? 1=A leads B, 0=B leads A
// To be completely out of phase, set shift=0 and invert_b=1

void startWaveforms( uint16_t half_period , uint16_t shift , uint8_t invert_b ) {

    TCCR1B = 0;     // Turn off counter if it is on. Makes sure nothing happens while we are getting things set up.
    // Leaves us in Normal mode, where we can do a Force Compare Match.

    // First we need to get the phases of the two outputs set up correctly

    // Set regs so when we force a compare it will match. This is the only way to set the state of the outputs on this chip.
    OCR1A = 0;
    OCR1B = 0;
    TCNT1 = 0;


    if (invert_b) {
        TCCR1A = _BV( COM1A1 ) | _BV( COM1B1 ) | _BV( COM1B0 );     // Output A=0 and B=1 on compare match (which we will force presently)
    } else {
        TCCR1A = _BV( COM1A1 ) | _BV( COM1B1 );                     // Output A=0 and B=0 on compare match (which we will force presently)
    }

    // Force a compare.
    // If polarity=0, then make A=1, B=1
    // If polarity=1, then make A=1, B=0
    // TCCR1C = _BV( FOC1A ) | _BV( FOC1B );

    // Now that polarity is established, we can set things ready to run

    // Both outputs into toggle mode. The output will toggle each time there is a compare match.
    // Since they both toggle once per period, they will stay in whatever polarity they start in.
    TCCR1A = _BV( COM1A0 ) |_BV( COM1B0 );

    OCR1A=0;
    OCR1B=shift;

    // Set the counter to roll over on the first step. This will force a match when it rolls to 0.
    TCNT1 = 0xffff;

    // In CTC Waveform Generation Mode
    // TCNT counts up to ICR1 then resets back to 0

    ICR1 = half_period-1;      // We subtract 1 becuase when we get to ICR1 then on the next tick we go back to 0, so will make `period` ticks per cycle total.

    // Turn on timer now
    // Mode=CTC, clock=clkio/1 (no prescaling)
    // Note: you could use a prescaller here for lower frequencies

    TCCR1B = _BV( WGM13) | _BV( WGM12) | _BV( CS10 );

}

// Stop output, make both outputs low.

void stopWaveforms() {

    TCCR1A = _BV( COM1A1 ) | _BV( COM1B1 ) ;                    // Output A=0 and B=0 on next compare match

}

// Min frequency is 16mhz / 65536 ticks per timer cyclke /2 timer cycles per waveform cycle  ~= 122 hz

// freq_hz is frequency in hertz from 122 to 4,000,000
// shift_deg is phase shift between output A and output B in degrees (values outside -180 to +180 are normalized)

// Note that at frequencies above 44KHz you will loose precision on phase shift. At 4Mhz, everything is rounded to just 0 or +/-90 degrees.
// Note that all timing happens in 1/16Mhz increments, so frequencies that do not land on integer multipules of that will be aproximiate.
// Use startWaveforms() directly for more precise control.

void startQuadrature( unsigned long freq_hz , int shift_deg  ) {

    // This assumes prescaler = 1. For lower freqnecies, use a larger prescaler.

    unsigned long clocks_per_toggle = (F_CPU / freq_hz) / 2;    // /2 becuase it takes 2 toggles to make a full wave

    // Normalize into 0-360 degrees

    while ( shift_deg < 0 ) {
        shift_deg += 360;
    }

    while ( shift_deg >= 360 ) {
        shift_deg -= 360;
    }

    // Normalize a shift of more than 180 degress

    uint8_t invert = 0 ;

    if (shift_deg >= 180 ) {

        invert = 1 ;   // Invert direction
        shift_deg -= 180;

    }

    unsigned long offset_clocks;

    offset_clocks = (clocks_per_toggle * shift_deg) / 180UL; // Do multiplication first to save precision

    startWaveforms( clocks_per_toggle , offset_clocks , invert );

}

void stopQuadrature() {
    stopWaveforms();
}


// Demo showing a good range of cases
// (like unit testing... but for hardware!)

// void loop() {
//
//     // 250Hz, B about 5us behind behind A
//     startQuadrature( 250 , 1);
//     delay(100);
//     stopQuadrature();
//     delay(1000);
//
//     // 1KHz, A and B exactly in phase
//     startQuadrature( 1000 , 0 );
//     delay(100);
//     stopQuadrature();
//     delay(1000);
//
//     // 10KHz, A and B exactly out of phase
//     startQuadrature( 10000 , 180 );     //(-180 same result)
//     delay(100);
//     stopQuadrature();
//     delay(1000);
//
//     // 100KHz, A leads B by 90 degrees
//     startQuadrature( 100000 , 90 );
//     delay(100);
//     stopQuadrature();
//     delay(1000);
//
//     // 1MHz, B leads A by 90 degrees
//     startQuadrature( 1000000 , -90 );
//     delay(100);
//     stopQuadrature();
//     delay(1000);
//
// }
