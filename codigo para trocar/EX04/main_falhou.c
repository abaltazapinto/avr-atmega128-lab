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
/* Display ativo a 0 */
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

/*
 * -------------------- Seleção dos displays --------------------
 * Assunção:
 * PD7:PD6 selecionam um dos 4 displays.
 *
 * Ajuste rápido se a ordem estiver errada:
 * troca os valores de DISP_H, DISP_T, DISP_U.
 *
 * Exemplo atual:
 *   3 -> display mais à esquerda
 *   2 -> segundo display
 *   1 -> terceiro display
 *   0 -> quarto display (não usado)
 */
#define DISP_H 1   /* centenas */
#define DISP_T 2   /* dezenas  */
#define DISP_U 3   /* unidades */

/* -------------------- Variáveis globais -------------------- */
volatile uint8_t tick_2ms = 0;

state_t state = IDLE;
uint16_t value = 0;          /* 0..599 */
uint8_t show_display = 1;

uint16_t cnt_10ms = 0;       /* 5 ticks de 2 ms = 10 ms */
uint16_t cnt_blink = 0;      /* 250 ticks = 500 ms */
uint16_t cnt_3s = 0;         /* 1500 ticks = 3 s */

static uint8_t current_display = 0;

/* -------------------- Funções -------------------- */
static void io_init(void)
{
	/* PORTC = segmentos */
	DDRC = 0xFF;
	PORTC = DISPLAY_OFF;

	/* PD0 e PD1 = inputs (SW1 e SW2) */
	DDRD &= ~((1 << PD0) | (1 << PD1));

	/* pull-up dos botões */
	PORTD |= (1 << PD0) | (1 << PD1);

	/* PD6 e PD7 = outputs (MUX dos displays) */
	DDRD |= (1 << PD6) | (1 << PD7);

	/* estado inicial */
	PORTD |= (1 << PD6) | (1 << PD7);
}

static void timer0_init(void)
{
	/* CTC mode */
	TCCR0 = (1 << WGM01);

	/*
	 * 16 MHz, prescaler 256:
	 * T = 256 / 16e6 * (OCR0 + 1)
	 * OCR0 = 124 -> 2 ms
	 */
	OCR0 = 124;

	/* enable compare match interrupt */
	TIMSK |= (1 << OCIE0);

	/* start timer with prescaler 256 */
	TCCR0 |= (1 << CS02);
}

static void select_display(uint8_t mux)
{
	/* limpa PD6 e PD7 */
	PORTD &= ~((1 << PD6) | (1 << PD7));

	/* escreve o valor de mux nos bits 7:6 */
	PORTD |= ((mux & 0x03) << 6);
}

static void display_value(uint16_t val, uint8_t visible)
{
	uint8_t digits[3];

	digits[0] = val / 100;         /* centenas */
	digits[1] = (val / 10) % 10;   /* dezenas */
	digits[2] = val % 10;          /* unidades */

	if (!visible)
	{
		PORTC = DISPLAY_OFF;

		/* mantém um MUX válido */
		select_display(DISP_U);
		return;
	}

	switch (current_display)
	{
		case 0:
			select_display(DISP_H);
			PORTC = seg_table[digits[0]];
			break;

		case 1:
			select_display(DISP_T);
			PORTC = seg_table[digits[1]];
			break;

		case 2:
			select_display(DISP_U);
			PORTC = seg_table[digits[2]];
			break;

		default:
			current_display = 0;
			select_display(DISP_H);
			PORTC = seg_table[digits[0]];
			break;
	}

	current_display++;
	if (current_display >= 3)
		current_display = 0;
}

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
						current_display = 0;
						show_display = 1;
						state = RUN;

						while (START_PRESSED)
							;
					}
					break;

				case RUN:
					cnt_10ms++;
					if (cnt_10ms >= 500)   /* especificação real: 10 ms */
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

						while (STOP_PRESSED)
							;
					}
					break;

				case BLINK:
					cnt_3s++;
					cnt_blink++;

					if (cnt_blink >= 250)   /* 500 ms ON/OFF -> 1 Hz */
					{
						cnt_blink = 0;
						show_display = !show_display;
					}

					if (cnt_3s >= 1500)     /* 3 s */
					{
						show_display = 1;
						state = FINAL;
					}
					break;

				case FINAL:
					/* valor fixo */
					break;

				default:
					state = IDLE;
					break;
			}
		}

		/* refresh multiplexado contínuo */
        display_value(value, show_display);
	}
}