/*
 * Exe1.c
 *
 * Created: 07/03/2026 09:54:48
 * Author : DEE
 */ 

#include <avr/io.h>

#define SW1	0b11111110
#define SW2	0b11111101
#define SW3	0b11111011
#define SW4	0b11110111
#define SW6	0b11011111

#define D1D8	0b01111110
#define D2D7	0b10111101
#define D3D6	0b11011011
#define D4D5	0b11100111

int main(void)
{
    /* Replace with your application code */
	DDRA = 0b00000000;
	PORTA = 0b00000000;
	DDRC = 0b11111111;
	PORTC = 0b11111111;
    while (1) 
    {
		if (PINA == SW1)
		{
			PORTC &= D1D8;//PORTC = PORTC & 0b01111110;
		}
		if (PINA == SW2)
		{
			PORTC &= D2D7;
		}
		if (PINA == SW3)
		{
			PORTC &=D3D6;
		}
		if (PINA == SW4)
		{
			PORTC &= D4D5;
		}
		if (PINA == SW6)
		{
			PORTC = 0b11111111;
		}
    }
}
