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
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t tick_2ms = 0;

ISR(TIMER0_COMP_vect)
{
    tick_2ms++;
}

static void timer0_init(void)
{
    /* Timer0 em CTC, base de 2 ms */
    TCCR0 = (1 << WGM01);   /* CTC */
    TCCR0 |= (1 << CS02);   /* prescaler 256 */
    OCR0 = 124;             /* 2 ms @ 16 MHz */
    TIMSK |= (1 << OCIE0);  /* enable compare interrupt */
    sei();
}

int main(void)
{
    uint8_t leds = 0xFF;
    uint16_t last_tick = 0;

    /* LEDs D1..D8 em PORTC */
    DDRC = 0xFF;
    PORTC = leds;

    /* PORTA: PA0..PA5 switches, PA6..PA7 controlo da board */
    DDRA  = 0b11000000;
    PORTA = 0b11000000;

    timer0_init();

    while (1)
    {
        uint8_t sw = PINA;

        /* SW1 -> sequência acumulativa D1 -> D8 com 1 s */
        if (!(sw & (1 << PA0)))
        {
            if ((uint16_t)(tick_2ms - last_tick) >= 500)
            {
                last_tick = tick_2ms;

                if (leds != 0x00)
                {
                    leds <<= 1;
                    PORTC = leds;
                }
            }
        }
        /* SW2 -> sequência inversa D8 -> D1 com 0.5 s */
        else if (!(sw & (1 << PA1)))
        {
            if ((uint16_t)(tick_2ms - last_tick) >= 250)
            {
                last_tick = tick_2ms;

                if (leds != 0xFF)
                {
                    leds = (leds >> 1) | 0x80; (MSB)
                    PORTC = leds;
                }
            }
        }
    }
}
