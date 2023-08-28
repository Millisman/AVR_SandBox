#ifndef QUADRATURE_H_INCLUDED
#define QUADRATURE_H_INCLUDED

#include <stdint.h>

typedef struct {
    uint8_t tenthous;
    uint8_t thous;
    uint8_t hundreds;
    uint8_t dozens;
    uint8_t units;
    uint32_t VALUE;
} digits_value_t;


typedef enum {
    // NONE,
    Number0,
    Number1,
    Number2,
    Number3,
    Number4
    // DIR_LEFT,
    // DIR_RIGHT
} mod_freq_5num;

typedef enum {
    SWV_NONE,
    SWV_INC,
    SWV_DEC,
    SWV_CLICK
} swv_t;


// void switch_value(swv_t sw);

void quadrature_init();


// // // // // void quadrature_set();

void quadrature_update();


void startQuadrature( unsigned long freq_hz , int shift_deg  );
void stopWaveforms();
void startWaveforms( uint16_t half_period , uint16_t shift , uint8_t invert_b );

void quadrature_freq_lcd();

void quadrature_turnover_lcd();







void quadrature_encoder_right();
void quadrature_encoder_left();
void quadrature_encoder_released_short();
void quadrature_encoder_released_long();


#endif // QUADRATURE_H_INCLUDED
