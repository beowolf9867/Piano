#include <avr/io.h>
#include <util/delay.h>
#include "note_table.h"

#define THRESHOLD_VALUE 800  
#define OCTAVE_OFFSET 36     

void setup(void) {
    // port c outputs
    DDRC |= 0x07; 
    
    // port a inputs
    DDRA &= ~(_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3)); 

    ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2); 
    ADMUX |= _BV(REFS0); 

    // square wave pd5
    TCCR1B |= _BV(WGM12) | _BV(CS11); 
    DDRD |= _BV(PD5); 
}

uint16_t read_adc(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= _BV(ADSC); 
    while (ADCSRA & _BV(ADSC)); 
    return ADC;
}

int main(void) {
    // disable jtag twice to unlock Pin 24 (PC2)
    MCUCR |= _BV(JTD);
    MCUCR |= _BV(JTD); 

    setup();

    while(1) {
        int16_t detected_key = -1;

        for (uint8_t i = 0; i < 8; i++) {
            PORTC = i; // sets S0, S1, S2 for all muxes
            _delay_us(10); // small delay for mux switching

            // check all 4 muxes
            for (uint8_t mux = 0; mux < 4; mux++) {
                if (read_adc(mux) < THRESHOLD_VALUE) {
                    detected_key = (mux * 8) + i; 
                    break;
                }
            }
            if (detected_key != -1) break;
        }

        if (detected_key != -1) {
            uint8_t note_index = OCTAVE_OFFSET + detected_key;
            if (note_index < 128) {
                OCR1A = note_table[note_index]; // set frequency
                TCCR1A |= _BV(COM1A0);          // sound on
            }
        } else {
            TCCR1A &= ~(_BV(COM1A0));           // sound off
        }
    }
}
