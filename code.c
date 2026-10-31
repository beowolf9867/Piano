#include <avr/io.h>
#include <util/delay.h>
#include "note_table.h"

#define THRESHOLD_VALUE 800
#define NUM_KEYS 25
#define OCTAVE_MIN -3
#define OCTAVE_MAX 3
#define OCTAVE_OFFSET 36

void setup(void) {
  DDRC |= _BV(PC0) | _BV(PC1) | _BV(PC2); 
  DDRA &= ~(_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3)); 
  DIDR0 |= _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D); // disable digital buffers

  ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2); // enable + Prescaler 64
  ADMUX |= _BV(REFS0); //reference voltage set

  TCCR1B |= _BV(WGM12) | _BV(CS11); //set prescaler and set ctc mode
  DDRD |= _BV(PD5); //headphone
}

int main(void) {
    setup();
    
    int8_t octave_shift = 0;
    uint8_t lock = 0;

    while(1) {
      uint8_t detected_key = -1;
      
      for (uint8_t i = 0; i < 8; i++) {
          PORTC = (PORTC & 0xF8) | (i & 0x07); //select mux channel
          _delay_us(10);
          
          for (uint8_t j = 0; j < 4; j++) { 
              ADMUX = (ADMUX & 0xF0) | (j & 0x0F); 
              ADCSRA |= _BV(ADSC); //start conversion
              while (ADCSRA & _BV(ADSC)); 
                
              uint16_t adc_val = ADC;

              if (adc_val < THRESHOLD_VALUE) {
                  int8_t temp_key = (j * 8) + i; 
                  if (temp_key < NUM_KEYS + 3) { 
                      detected_key = temp_key; 
                      break;
                  }
              }
          }
          if (detected_key != -1) 
            break; 
      }

      if (detected_key == 25) { // octave Up
          if (lock == 0) {
              if (octave_shift < OCTAVE_MAX) 
                octave_shift++;
              lock = 1;
          }
          detected_key = -1; // no note played for octave control keys
      } 
      else if (detected_key == 26) { // octave Down
          if (lock == 0) {
              if (octave_shift > OCTAVE_MIN) 
                octave_shift--;
              lock = 1;
          }
          detected_key = -1;
      } 
      else if (detected_key == 27) { // home
          if (lock == 0) {
              octave_shift = 0;
              lock = 1;
          }
          detected_key = -1;
      } 
      else if (detected_key == -1) {
          lock = 0; 
      }

      
      if (detected_key != -1 && detected_key < 25) {
          int16_t target = OCTAVE_OFFSET + detected_key + (12 * octave_shift);
          
          if (target < 0) target = 0;
          if (target > 127) target = 127;

          OCR1A = note_table[target];
          TCCR1A |= _BV(COM1A0); //connect toggle to pd5
      } else {
          TCCR1A &= ~_BV(COM1A0); //silence
          PORTD &= ~_BV(PD5); //0v
      }
    }
    return 0;
}
