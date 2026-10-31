#include <avr/io.h>
#include <util/delay.h>
#include "note_table2.h"

#define THRESHOLD_VALUE 800
#define NUM_KEYS 25

void setup(void) {
  DDRC |= _BV(PC0) | _BV(PC1) | _BV(PC2); 
  DIDR0 |= _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D); // disable digital buffers
MCUCR |= _BV(JTD); MCUCR |= _BV(JTD);
  ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2); //(enable + Prescaler 64)
  ADMUX |= _BV(REFS0); 
DDRA &= ~_BV(PA0);
 ADMUX = (ADMUX & 0xF0) | (0 & 0x0F); // adc at channel 0
  TCCR1B |= _BV(WGM12) | _BV(CS11); //set prescaler and set ctc mode
  DDRD |= _BV(PD5); //headphone
}


int main(void) {
    setup();
    while (1) {
      int8_t detected_key = -1;
      for (uint8_t i = 0; i < 8; i++) {
          PORTC = (PORTC & 0xF8) | (i & 0x07); /
          _delay_us(20);
          ADCSRA |= _BV(ADSC); 
	  while (ADCSRA & _BV(ADSC)); 
	  uint16_t adc_val = ADC;
	  if (adc_val < THRESHOLD_VALUE) {
	    int8_t temp_key = i; 
	    if (temp_key < NUM_KEYS) {
	      detected_key = temp_key; 
	      break;
	    }
	  }
	  if (detected_key != -1) break;
      }
      if (detected_key != -1) {
	int16_t target = detected_key;
          OCR1A = note_table[60+target];
          TCCR1A |= _BV(COM1A0);
      } else {
          TCCR1A &= ~_BV(COM1A0); 
          PORTD &= ~_BV(PD5); 
      }
    return 0; 
    }
}
