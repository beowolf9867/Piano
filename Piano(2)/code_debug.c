#include <avr/io.h>
#include <util/delay.h>
#include "note_table.h"

#define THRESHOLD 800 
#define START_NOTE 36 

void setup(void) {
    // 1. Port Directions
    DDRC |= 0x07;   // PC0, PC1, PC2 as OUTPUTS (Mux Addresses S0, S1, S2)
    DDRA &= 0xF0;   // PA0, PA1, PA2, PA3 as INPUTS 
    DDRD |= _BV(PD5); // speaker

    // REFS0: AVCC with external capacitor at AREF pin
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1); // Enable ADC, Prescaler 64
    ADMUX |= _BV(REFS0); 

    TCCR1B |= _BV(WGM12) | _BV(CS11); // CTC mode, Prescaler 8
}

uint16_t read_adc(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    ADCSRA |= _BV(ADSC);
    
    while (ADCSRA & _BV(ADSC));
    
    return ADC;
}

int main(void) {
    // disable JTAG twice to unlock Pin 24 (PC2)
    MCUCR |= _BV(JTD);
    MCUCR |= _BV(JTD); 

    setup();

    while(1) {
        int16_t active_key = -1;

        for (uint8_t i = 0; i < 8; i++) {
            PORTC = i;         // Set S0, S1, S2 for all muxes
            _delay_us(10);     // brief delay for electrical switching

            for (uint8_t m = 0; m < 4; m++) {
                if (read_adc(m) < THRESHOLD) {
                    active_key = (m * 8) + i; 
                    break;
                }
            }
            if (active_key != -1) 
              break; 
        }

        if (active_key != -1) {
            uint16_t note_index = START_NOTE + active_key;
            OCR1A = note_table[note_index];
            TCCR1A |= _BV(COM1A0); 
        } else {
            TCCR1A &= ~(_BV(COM1A0)); 
        }
    }
}
