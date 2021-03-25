#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>												 // Needed for uint8_t
#include <util/delay.h>

const char DisplayCC[10] = {0b0111111, 0b0000110,
	0b1011011, 0b1001111,
	0b1100110, 0b1101101,
	0b1111101, 0b0000111,
	0b1111111, 0b1101111};
const int nDigit = 3;											 // Number of digits shown in Display
unsigned int adc_value;
// Global variables

void ADC_Init() {
	ADMUX=(1<<REFS0)|(1<<1);									 // Selecting AVCC reference (5V) and channel A0 as analog input
	ADCSRA=(1<<ADIE)|(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Enable ADC also set Prescaler as 128 and AD interruptions are enabled
	ADCSRA|=(1<<ADSC);											 // ADSC is set to 1 to start next conversion
}

void Init_System() {
	/*------1 for outputs and 0 for inputs-----*/

	DDRC=0b11111111;											 // 7 segments of display are setup as output pins
	DDRA=0b00000111;											 // 3 digital GND are setup as outputs to multiplex which digit is shown in display
	DDRF=0b11111110;											 // Potenciometer analog input
	
	ADC_Init();													// ADC pins are initialized
	sei();														// Enables interruptions
}

int main(void)
{
	int j,k;
	char* pDisplayCC=&DisplayCC;								// pDisplayCC is initialized to point to the address of DisplayCC
	Init_System();
	float pot[nDigit];
	char poti[nDigit];
	while(1) {
		
		for (j=0;j<nDigit;j++)
		{
			pot[0]=(adc_value*5.0)/1024;						// First float value is read from ADC input
			poti[0]=trunc(pot[0]);								// First float value is truncated to integer
			for (k=1;k<nDigit;k++)								// The following algorithm initializes all values to be shown on display converting each decimal in integer
			{
				pot[k]=10*(pot[k-1]-trunc(pot[k-1]));
				poti[k]=trunc(pot[k]);
			}
			_delay_ms(1.5);
			PORTC=0x00;
			_delay_ms(1.5);
			
			if (j==0)											// First digit
			{
				PORTC=*(pDisplayCC+poti[j]) | 0x80;
				PORTA=0xFB;
			}
			else
			{													// Rest of digits
				PORTC=*(pDisplayCC+poti[j]) & 0x7F;
				PORTA=(0xFB >> j) | 0xF8;
			}
		}
	}
}																// Program being interrupted by ISR

ISR(ADC_vect) {
	adc_value=ADC;
	ADCSRA |= (1<<ADSC);										// ADSC is set to 1 to start next conversion
}