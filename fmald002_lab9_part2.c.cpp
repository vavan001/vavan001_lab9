/*	Author: fmald002
 *  Partner(s) Name: Robert Rivera
 *	Lab Section: 22
 *	Assignment: Lab # 9 Exercise #2
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

enum states{start, off, offToWait, wait, up, down, offWait} state;
const double sounds[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char i = 0;

void Tick() {
	unsigned char A1 = ~PINA & 0x01;
	unsigned char A2 = ~PINA & 0x02;
	unsigned char A3 = ~PINA & 0x04;
	
	switch(state) {
		case start:
			i = 0;
			state = off;
			break;
			
		case off:
			if (A3) {
				state = offToWait;
			} else {
				state = off;
			}
			break;
			
		case offToWait:
			if (A3) {
				state = offToWait;
			} else {
				state = wait;
			}
			break;
			
		case wait:
			if (A1 && i < 8) {
				++i;
				state = up;
			} else if (A2 && i > 0) {
				--i;
				state = down;
			} else {
				state = wait;
			}
			
			if (A3) {
				state = offWait;
			}
			break;
			
		case up:
			if (A1) {
				state = up;
			} else {
				state = wait;
			}
			
			if (A3) {
				state = offWait;
			}
			break;
			
		case down:
			if (A2) {
				state = down;
			} else {
				state = wait;
			}
			
			if (A3) {
				state = offWait;
			}
		break;
		
		case offWait:
			if (A3) {
				state = offWait;
			} else {
				state = off;
			}
			break;
		
		default:
			break;
	}
	
	switch(state) {
		case start:
		break;
		
		case off:
			set_PWM(0);
			break;
		
		case wait:
			set_PWM(sounds[i]);
			break;
			
		case up:
		set_PWM(sounds[i]);
		break;
		
		case down:
		set_PWM(sounds[i]);
		break;
		
		case offWait:
			//set_PWM(0);
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

