#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>										// needed for uint8_t
#include <util/delay.h>

unsigned int ADC_read(unsigned char chnl)
{
	chnl=chnl & 0b00000111;								// Select adc channel between 0 to 7
	ADMUX=0x40;											// Channel A0 selected
	ADCSRA|=(1<<ADSC);									// Start conversion
	while(!(ADCSRA & (1<<ADIF)));						// Wait for ADIF conversion complete return
	ADCSRA|=(1<<ADIF);									// Clear ADIF when conversion complete by writing 1
	return (ADC);										// Return calculated ADC value
}														
// Source: https://circuitdigest.com/microcontroller-projects/how-to-use-adc-in-avr-microcontroller-atmega16

/*ISR(ADC_vect) {
	
	unsigned char adc_value = ADC;
	
	PORTE = adc_value;
	PORTH = adc_value >> 8;
	
	ADCSRA |= (1 << ADSC);
}*/
// Source: https://vidaembebida.wordpress.com/2014/07/29/avr-programacion-en-c-06-interrupciones-en-los-avr/
 
void ADC_Init() {
	ADMUX=(1<<REFS0);									   // Selecting internal reference voltage
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);     // Enable ADC also set Prescaler as 128
}

void Init_System() {
	/*------1 for outputs and 0 for inputs-----*/

	DDRC=0b11111111;										// 7 segments of display are setup as output pins
	DDRA=0b00000111;										// 3 digital GND are setup as outputs to multiplex which digit is shown in display

	//Analog input
	DDRF=0b11111110;										// Potenciometer analog input

	const int nDigit = 3;									// Number of digits shown in Display
	const uint8_t DisplayCC[10] = {0b0111111, 0b0000110,
	0b1011011, 0b1001111,
	0b1100110, 0b1101101,
	0b1111101, 0b0000111,
	0b1111111, 0b1101111};
	ADC_Init();												// ADC pins are initialized
	sei();													// Interruptions are enabled
}

int main(void)
{
Init_System();
	
uint8_t* pDisplayCC=&DisplayCC;								// pDisplayCC is initialized to point to the address of DisplayCC

	 
float pot[nDigit];
uint8_t poti[nDigit];   
int j,k;
	while (1) {
		
		pot[0]=(ADC_read(PINF0)*5.0)/1024;			   // First float value is read from ADC input
		poti[0]=trunc(pot[0]);						   // First float value is truncated to integer
		
		for (j=0;j<nDigit;j++)
		{
			for (k=1;k<nDigit;k++)					   // The following algorithm initializes all values to be shown on display converting each decimal in integer
			{
				pot[k]=10*(pot[k-1]-trunc(pot[k-1]));
				poti[k]=trunc(pot[k]);
			}
			
			if (j==0)									// First digit
			{
				PORTC=*(pDisplayCC+poti[j]) | 0x80;
				PORTA=0xFB;
				_delay_ms(3);
			}
			else {										// Rest of digits
				PORTC=*(pDisplayCC+poti[j]) & 0x7F;
				PORTA=(0xFB >> j) | 0xF8;
				_delay_ms(3);
			}												
				
		}
    }													// Program being interrupted by PINE0
}

