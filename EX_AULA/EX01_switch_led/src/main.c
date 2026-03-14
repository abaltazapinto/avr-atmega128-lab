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
/*/*
 * Exe1.c
 *
 * Created: 07/03/2026 09:54:48
 * Author : DEE
 */ 



ledStatus ledStatus_var = STATUS_INC;
movStatus flag_mov = STATUS_INC_ENABLE;
switchObj swObj;


uint8_t vec[8] = {D1, D2, D3, D4, D5, D6, D7, D8};
uint8_t vec_pos = 0x00;

void config_timer(void);
void update_port_c(void);
void update_state_machine(void);
void execute_count_routine(void);

volatile uint16_t count = 0x00;

int main(void)
{
    /* Replace with your application code */
	DDRC = 0b11111111;
	PORTC = 0xB0;
	DDRD = 0b11000000;
	PORTD = 0x00;
	
	TCCR0 = 0b00001101;
	OCR0 = 249;
	TIMSK = 0b00000010;
	
	sei();
	
    while (1) 
    {

    }
}

void config_timer(void)
{
	TCCR0 |= (1 << CS02);
	TCCR0 &= ~(1 << CS01);
	TCCR0 &= ~(1 << CS00);
	OCR0 = 249;
	TIMSK |= (1 << TOIE0);
}

ISR(TIMER0_COMP_vect)
{
	/*count++;
	if(count == 1000)
	{
		count = 0;
		PORTD += 0x40;
	}*/
	
	PORTD += 0x40;
}


void update_port_c(void)
{
	static uint8_t port_c_var = 0xFF;
	uint8_t aux = 0x00;
	
	if(ledStatus_var == STATUS_INC)
	{
		port_c_var &= vec[vec_pos];	
	}
	if(ledStatus_var == STATUS_DEC)
	{
		aux = (~vec[vec_pos]);
		port_c_var |= aux;	
	}
	
	PORTC = vec[vec_pos]; //port_c_var;
}

void update_state_machine(void)
{
	if(swObj.varSW6 == 1)
	{
		if(flag_mov == STATUS_INC_ENABLE)
		{
			flag_mov = STATUS_INC_DISABLE;
		}
		else
		{
			flag_mov = STATUS_INC_ENABLE;
		}
		swObj.varSW6 = 0;
	}   Clock Frequency  : ate 16 MHZ                                                                    */


}    /*                                                                                          */
/* **************************************************************************************** */





