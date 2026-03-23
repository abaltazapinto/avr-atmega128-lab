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
/*                                                                                          */
/* **************************************************************************************** */
/*                                                                                          */
/*   Target Hardware                                                                        */
/*   -------------------------------------------------------------------------------        */
/*   Microcontroller : ATmega128                                                                     */
/*   Architecture     : 8-bit AVR RISC                                                                    */
/*   Clock Frequency  : ate 16 MHZ                                                                    */
/*                                                                                          */
/* **************************************************************************************** */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/* -------------------- Estados -------------------- */
typedef enum
{
	IDLE = 0,
	RUN,
	BLINK,
	FINAL
} state_t;

/* -------------------- Tabela 7 segmentos -------------------- */
/* Display ativo a 0, conforme enunciado */
static const uint8_t seg_table[10] =
{
	0xC0, /* 0 */
	0xF9, /* 1 */
	0xA4, /* 2 */
	0xB0, /* 3 */
	0x99, /* 4 */
	0x92, /* 5 */
	0x82, /* 6 */
	0xF8, /* 7 */
	0x80, /* 8 */
	0x90  /* 9 */
};

#define DISPLAY_OFF 0xFF

/* -------------------- Botões -------------------- */
/* SW1 = Start = PD0 ; SW2 = Stop = PD1 */
#define START_PRESSED (!(PIND & (1 << PD0)))
#define STOP_PRESSED  (!(PIND & (1 << PD1)))

/* -------------------- Variáveis globais -------------------- */
volatile uint8_t tick_2ms = 0;

state_t state = IDLE;
uint16_t value = 0;          /* 0..599 */
uint8_t show_display = 1;

uint16_t cnt_10ms = 0;       /* 5 ticks de 2 ms */
uint16_t cnt_blink = 0;      /* 250 ticks = 500 ms */
uint16_t cnt_3s = 0;         /* 1500 ticks = 3 s */

/* -------------------- Funções -------------------- */

/*Primeira tentativa

static void io_init(void)
{
	/* PORTC = display -> output */
//	DDRC = 0xFF;
//	PORTC = DISPLAY_OFF;

	/* PD0 e PD1 como input */
//	DDRD &= ~((1 << PD0) | (1 << PD1));

	/* pull-up internos nos botões */
	//PORTD |= (1 << PD6) | (1 << PD7);
	
	// PD6 = 0, PD7 = 1
//PORTD &= ~(1 << PD6);
//PORTD |= (1 << PD7);
	/* selecionar display da direita: PD6 e PD7 a 1 */

	/* PD6 e PD7 como output (seleção de display) */
//	DDRD |= (1 << PD6) | (1 << PD7);
	/* começar com ambos a 1 (estado que já sabes que acende um display) */
//	PORTD |= (1 << PD6) | (1 << PD7);
//}
// so um display 
static void io_init(void)
{
	/* PORTC = segmentos */
	DDRC = 0xFF;
PORTC = DISPLAY_OFF;

	/* PD0 e PD1 = inputs (SW1 e SW2) */
	DDRD &= ~((1 << PD0) | (1 << PD1));

	/* pull-up dos botões */
	PORTD |= (1 << PD0) | (1 << PD1);

	/* PD6 e PD7 = outputs (seleção de display) */
	DDRD |= (1 << PD6) | (1 << PD7);

	/* estado inicial de seleção */
	PORTD |= (1 << PD6) | (1 << PD7);
}

static void timer0_init(void)
{
	/* CTC mode: WGM01 = 1 */
	TCCR0 = (1 << WGM01);

	/*
	 * Para 16 MHz:
	 * T = prescaler / F_CPU * (OCR0 + 1)
	 * Se prescaler = 256:
	 * 2 ms = 256 / 16e6 * (OCR0 + 1)
	 * OCR0 = 124
	 */
	OCR0 = 124;

	/* enable compare match interrupt */
	TIMSK |= (1 << OCIE0);

	/* start timer with prescaler 256 */
	TCCR0 |= (1 << CS02);
}



static void display_digit(uint16_t val, uint8_t visible)
{
	if (!visible)
	{
		PORTC = DISPLAY_OFF;
		return;
	}

	PORTC = /*seg_table[1];*/ seg_table[val % 10];
}


//static void display_value(uint16_t val, uint8_t visible)
//{
//	uint8_t digits[3];

//	digits[0] = val / 100;         /* centenas */
//	digits[1] = (val / 10) % 10;   /* dezenas */
//	digits[2] = val % 10;          /* unidades */

	//if (!visible)
//	{
	//	/* aqui depois apagas os 3 displays */
	//	return;
//	}

	/* aqui depois vais multiplexar:
	   display 0 -> seg_table[digits[0]]
	   display 1 -> seg_table[digits[1]]
	   display 2 -> seg_table[digits[2]]
	*/
//}

ISR(TIMER0_COMP_vect)
{
	tick_2ms = 1;
}

int main(void)
{
	io_init();
	timer0_init();
	sei();

	while (1)
	{
		if (tick_2ms)
		{
			tick_2ms = 0;

			switch (state)
			{
				case IDLE:
					show_display = 0;
					if (START_PRESSED)
					{
						value = 0;
						cnt_10ms = 0;
						cnt_blink = 0;
						cnt_3s = 0;
						show_display = 1;
						state = RUN;

						while (START_PRESSED)
							;
					}
					break;

				case RUN:
					cnt_10ms++;
					if (cnt_10ms >= 1500)
					{
						cnt_10ms = 0;
						value++;
						if (value >= 600)
							value = 0;
					}

					if (STOP_PRESSED)
					{
						cnt_blink = 0;
						cnt_3s = 0;
						show_display = 1;
						state = BLINK;
					}
					break;

				case BLINK:
					cnt_3s++;
					cnt_blink++;

					if (cnt_blink >= 250)
					{
						cnt_blink = 0;
						show_display = !show_display;
					}

					if (cnt_3s >= 1500)
					{
						show_display = 1;
						state = FINAL;
					}
					break;

				case FINAL:
					/* fica fixo */
					break;

				default:
					state = IDLE;
					break;
			}
		}
		//display_value(value, show_display);
		 display_digit(value, show_display);
	}
}