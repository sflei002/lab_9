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


volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
        TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0x02;

        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}



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

enum States {init, off, on, wait} state;
unsigned char b = 0x00;
unsigned char i = 0x00;
unsigned char cnt = 0x00;
double notes[] = {261.63, 523.25, 293.66, 493.88, 329.63, 440.00, 349.23, 392.00, 523.25, 261.63};
double order[] = {1, 1, 2, 2, 1, 1, 2, 2, 1, 1};
void Tick(){
	b = ~PINA & 0x01;
	switch(state){
		case init:
			state = off;
			break;
		case off:
			state = b ? on : off;
			break;
		case on:
                        state = (i < 10) ? on : wait;
			break;
		case wait:
			state = b ? wait : off;
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
			i = 0x00;
			cnt = 0x00;
                        break;
                case on:
                        if(cnt < order[i]){
				set_PWM(notes[i]);
				++cnt;
			}
			else{
				set_PWM(notes[i]);
				++i;
				cnt = 0x01;
			}
                        break;
                case wait:
			set_PWM(0);
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
TimerOn();
TimerSet(45);
state = -1;
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag){}
	TimerFlag = 0;
    }
}
