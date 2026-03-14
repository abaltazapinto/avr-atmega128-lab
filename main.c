/* **************************************************************************************** */
/*                                                                                          */
/*   File:        main.c                                                      */
/*   Author:      Andre Pinto                                               */
/*   Email:       1200209@student.isep.ipp.pt                               */
/*                                                                                         */
/*   Course:      Embedded Engineering                                                      */
/*   Institution: ISEP                                                                      */
/*                                                                                          */
/*   Created:     2026/03/13 18:07                                              */
/*   Updated:     2026/03/13 18:07                                              */
/*                                                                                          */
/* **************************************************************************************** */
/*                                                                                          */
/*   Module Description                                                                     */
/*
       EX02 – LED sequence

	Objetivo:
		Criar uma sequência de LEDs no PORTC.

	Ideia da lógica:

		1. configurar PORTC como output
		2. iniciar estado LED = 0b00000001
		3. loop infinito
		4. deslocar bit para a esquerda
		5. quando chegar ao último LED, reiniciar sequência
		6. inserir delay entre mudanças
                                                                                       */
/* **************************************************************************************** */
/*                                                                                          */
/*   Target Hardware                                                                        */
/*   -------------------------------------------------------------------------------        */
/*   Microcontroller : ATmega128                                                                     */
/*   Architecture     : 8-bit AVR RISC                                                                    */
/*   Clock Frequency  : ate 16 MHZ                                                                    */
/*                                                                                          */
/* **************************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void timer0_init(void)
{
    /* CTC mode */
    TCCR0 = (1 << WGM01);

    /* prescaler = 256 */
    TCCR0 |= (1 << CS02);

    /* compare value para 2 ms */
    OCR0 = 124;

    /* enable compare interrupt */
    TIMSK |= (1 << OCIE0);

    /* enable global interrupts */
    sei();
}


volatile uint16_t tick_2ms = 0;

ISR(TIMER0_COMP_vect)
{
	tick_2ms++;
}

int main(void)
{
	/* LEDs -> PORTC (output) */
	DDRC = 0xFF;
	/* switches -> PORTA (input) */
	DDRA = 0b11000000;
	/* pull-ups para switches */
	PORTA = 0b11000000;
	/* todos os LEDs apagados (active-low) */
	uint8_t leds = 0xFF;
	PORTC = leds; // todos apagados supostamente (active low)
	uint16_t last_tick = 0;
	timer0_init();
	while (1)
	{
		/*
		uint8_t sw = PINA;
	/* SW1 pressionado (active-low) -> ligar D1 ate d8 acumulativamente */
	/*	if (!(sw & (1<< PA0)))
		{
		// sequência crescente D1 -> D8
			if((uint16_t)(tick_2ms - last_tick) >= 500)
			{
				last_tick = tick_2ms;

				if (leds != 0x00)
				{
					leds = (leds << 1);
					PORTC = leds;
				}
			}
		}

		/* SW2 pressionado (active-low) */
	/*	else if (!(sw & (1 << PA1)))
		{
		// sequência inversa D8 -> D1
			if ((uint16_t)(tick_2ms - last_tick) >= 250)
			{
				last_tick = tick_2ms;

				if(leds != 0xFF)
				{
					leds = (leds >> 1) | 0x80;
					PORTC = leds;
				}
			}
		}*/
		DDRC = 0xFF;
		DDRA = 0b11000000;
		PORTA = 0b11000000;
		PORTC = 0xFF;

    	}
}
