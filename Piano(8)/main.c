#include <avr/io.h>
#include <util/delay.h>

#define PRESCALER 8
#define F_CPU     12000000

void setup_adc(void);
int read_adc(uint8_t channel);
void setup_timer(void);
void generate_music(uint16_t n);

#define THRESHOLD_VALUE 128

int main(void) {
  setup_adc();
  setup_timer();
  DDRD = 0xFF;
  PORTD = 0x00;
  while (1) {
    for (uint8_t n = 0; n <= 7; n++) {
      uint8_t adc_value = read_adc(n);
      if (adc_value < THRESHOLD_VALUE) {
	TCCR1A |= _BV(COM1A0);
	switch (n) {
	case 0:
	  generate_music(262);
	  break;
	case 1:
	  generate_music(294);
	  break;
	case 2:
	  generate_music(330);
	  break;
	case 3:
	  generate_music(349);
	  break;
	case 4:
	  generate_music(392);
	  break;
	case 5:
	  generate_music(440);
	  break;
	case 6:
	  generate_music(494);
	  break;
	case 7:
	  generate_music(523);
	  break;
	}
	break;
      }
      else {
	TCCR1A &= ~_BV(COM1A0);
	PORTD &= ~(1 << n);
      }
    }
    _delay_ms(200);
  }
}

void setup_adc(void) {
  ADCSRA |= _BV(ADEN); //enable
  ADMUX |= _BV(REFS0); //ref voltage: 2.56v
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1); //prescaler of 64
  ADMUX |= _BV(ADLAR); //left adjusted
}

int read_adc(uint8_t channel) {
  ADMUX = (ADMUX & 0xF0) | (channel & 0x07); //MUX4:0 to channel
  ADCSRA |= _BV(ADSC); //start single conversion
  while(ADCSRA & _BV(ADSC));
  return ADCH;
}

void setup_timer(void) {
  DDRD = 0xFF;
  PORTD |= _BV(PD5); //output

  TCCR1A |= _BV(COM1A0) | _BV(WGM10);
  TCCR1B |= _BV(WGM13) | _BV(CS11); 
}

void generate_music(uint16_t n) {
  uint16_t f = (uint16_t)(F_CPU / (2 * PRESCALER * 2 * n));
  OCR1A = f;
}
