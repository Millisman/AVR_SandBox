#include "adc.h"

/*  case ADC_1V1: // 1v1    ADM*UX |= ((1 << REFS1) | (1 << REFS0));
    case ADC_AREF: // ADC_AREF (default)    ADMUX &= ~((1 << REFS1) | (1 << REFS0));
    case ADC_VCC: // ADC_VCC    ADMUX &= ~(1 << REFS1); ADMUX |= (1 << REFS0);
*/

#define ADC_REFERENCE REFS0
