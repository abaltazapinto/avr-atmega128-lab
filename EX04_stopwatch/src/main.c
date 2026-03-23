/*
 * PGSCE_Exercice2.c
 *
 * Created: 14/03/2026 11:07:56
 * Author : DEE
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SW1		0b11111110
#define SW2		0b11111101

uint8_t seg[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
uint8_t display[] = {0x40, 0x80, 0xC0};


uint8_t count = 0;

uint16_t numbercount1 = 0;
uint16_t numbercount2 = 0;
uint16_t numbercount3 = 0;
uint16_t totalcounter = 0;
uint16_t stopcounter = 0;
uint8_t vector_number[4];
uint8_t button_press = 0;

ISR(TIMER0_COMP_vect){

	if(button_press == 0){
		totalcounter += 1;

		vector_number[0] = totalcounter /1000;
		vector_number[1] = (totalcounter/100)%10;
		vector_number[2] = (totalcounter/10) % 10;
		vector_number[3] = (totalcounter % 10);
		
		
		if(totalcounter >= 6000){
			totalcounter = 0;
		}
		if(count > 2){
			count = 0;
		}
		
		PORTD = display[count];
		PORTC = seg[vector_number[count]];
		count += 1;
	}
	if(button_press == 1){
		stopcounter += 1;
		if(stopcounter >= 100 && stopcounter<300){
			if(count > 2){
				count = 0;
			}
		PORTD = display[count];
		PORTC = seg[vector_number[count]];
		count += 1;
		}
		if(stopcounter >= 300){
			TIMSK &= ~(1<<OCIE0);
			//sei();
			
			
		}
		
	}

}

ISR(INT1_vect) {
	PORTA ^= ~0b11111101;
	//TIMSK &= ~(1<<OCIE0);
	//sei();
	button_press = 1;
	count = 0;
	
}

ISR(INT0_vect) {
	PORTA ^= ~0b11111110;
	TIMSK |= (1<<OCIE0);
	sei();
	button_press = 0;
			
}

int main(void)
{
    /* Replace with your application code */
	//TCCR
	DDRC	= 0b11111111;
	PORTC	= 0xC0;
	DDRA	= 0b11111111;
	PORTA	= 0b11111111;
	
	DDRD	= 0b11110000;
	PORTD	= 0b11110000;
	
	//PORTD |= (1 << PD2); // Enable internal pull-up on PD2
	EICRA |= (1 << ISC11); //Falling edge => 10 on EICRA5:4
	EIMSK |= (1 << INT1); // Enable INT2 (PD2)
	EICRA |= (1 << ISC01); //Falling edge => 10 on EICRA5:4
	EIMSK |= (1 << INT0); // Enable INT2 (PD2)
	sei(); // Enable interrupts globally
	
	//sprintf(display, "%d", 1234);
	
	
	
	//DDRA = 0b11000000;
	//PORTA = 0b11000000;
	//PORTD	= 0b00000011;

	
	uint8_t counter_flag = 0;
	TCCR0 |= (1<<WGM01);
	TCCR0 |= (1<<CS02) | (1<<CS00);
	

	
	OCR0 = 249;
	

	
    while (1) 
    {

	if(button_press == 1){
		if(stopcounter >=300){
			PORTD = display[0];
			PORTC = seg[vector_number[0]];
			PORTD = display[1];
			PORTC = seg[vector_number[1]];
			PORTD = display[2];
			PORTC = seg[vector_number[2]];
		}
	  }
	}
}

