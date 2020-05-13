/*	Author: sflei002
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){
		if(!frequency){
			TCCR3B &= 0x08;
		}
		else{
			TCCR3B |= 0x03;
		}
		if(frequency < 0.954){
			OCR3A = 0xFFFF;
		}
		else if(frequency > 31250){
			OCR3A = 0x0000;
		}
		else{
			OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		}
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States {init, off, on1, on2, off1, inc, dec} state;
unsigned char b = 0x00;
unsigned char i = 0x00;
double notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
void Tick(){
	b = ~PINA & 0x07;
	switch(state){
		case init:
			state = off;
			break;
		case off:
			state = (b == 0x04) ? on1 : off;
			break;
		case on1:
                        state = (b == 0x00) ? on2 : on1;
			break;
		case on2:
			if(b == 0x01){
				if(i < 7){
					i++;
				}
				state = inc;
			}
			else if(b == 0x02){
				if(i > 0){
					i--;
				}
				state = dec;
			}
			else if(b == 0x04){
				state = off1;
			}
			break;
		case off1:
                        state = (b == 0x00) ? off : off1;
			break;
		case inc:
                        state = (b == 0x00) ? on2 : inc;
                        break;
		case dec:
                        state = (b == 0x00) ? on2 : inc;
                        break;
		default:
			state = init;
			break;
	}



	switch(state){
                case init:
                        break;
                case off:
                        set_PWM(0);
			state = off;
                        break;
                case on1:
                        set_PWM(notes[i]);
                        break;
                case on2:
                        break;
                case off1:
                        set_PWM(0);
                        break;
                case inc:
                        set_PWM(notes[i]);
                        break;
                case dec:
                        set_PWM(notes[i]);
                        break;
                default:
                        state = init;
                        break;
        }
}


int main(void) {
    /* Insert DDR and PORT initializations */
DDRA = 0x00;
PORTA = 0xFF;
DDRB = 0xFF;
PORTB = 0x00;
PWM_on();
state = -1;
    /* Insert your solution below */
    while (1) {
	Tick();
    }
}
