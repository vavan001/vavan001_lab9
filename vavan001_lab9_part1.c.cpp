/*	Author: Vikhyath Avantsa
 *  Partner(s) Name: Robert Rivera and Francisco Maldonado 
 *	Lab Section: 22
 *	Assignment: Lab # 9 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) {
			TCCR3B &= 0x08;
		} else {
			TCCR3B |= 0x03;
		}
		
		if (frequency < 0.954) {
			OCR3A = 0xFFFF;
		} else if (frequency > 31250) {
			OCR3A = 0x0000;
		} else {
			OCR3A = (short)(8000000/(128*frequency)) - 1;
		}
		
		TCNT3 = 0;
		current_frequency = frequency;
		}
}

void PWM_on() {
	TCCR3A = (1<<COM3A0);
	TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
	
}

enum states{start, wait, one, two, three, buttons} state;

void Tick() {
	unsigned char A1 = ~PINA & 0x01;
	unsigned char A2 = ~PINA & 0x02;
	unsigned char A3 = ~PINA & 0x04;
	
	switch(state) {
		case start: 
			state = wait;
			break;
		
		case wait:
			if (A1) {
				state = one;
			} else if (A2) {
				state = two;
				
			} else if (A3) {
				state = three;
			} else {
				state = wait;
				
			}
			break;
			
		case one:
			if (A1 && (A2 || A3)) {
				state = buttons;
			} else if (A1) {
				state = one;
			} else {
				state = wait;
			}
			break;
			
		case two:
			if (A2 && (A1 || A3)) {
				state = buttons;
			} else if (A2) {
				state = two;
			} else {
				state = wait;
			}
			break;
			
		case three:
			if (A3 && (A1 || A2)) {
				state = buttons;
			} else if (A3) {
				state = three;
			} else {
				state = wait;
			}
			break;
			
		case buttons:
			if (A1 && !A2 && !A3) {
				state = wait;
			} else if (!A1 && A2 && !A3) {
				state = wait;
			} else if (!A1 && !A2 && A3) {
				state = wait;
			} else {
				state = buttons;
			}
			break;
			
		default:
			state = wait;
			break;
	}
	
	switch(state) {
		case wait: 
			set_PWM(0);
			break;
		
		case one:
			set_PWM(261.63);
			break;
		
		case two:
			set_PWM(293.66);
			break;
			
		case three:
			set_PWM(329.63);
			break;
		
		case buttons:
			set_PWM(0);
			break;
		
		default:
			set_PWM(0);
			break;
	}
}

int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();
	state = start;
    while (1) {
		Tick();
	}
}

