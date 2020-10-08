/*
 * WaveformGenerator.cpp
 *
 *  Created on: Oct 22, 2014
 *      Author: gonzagasilvas
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// Initializes all pins which will be used
void initPorts(void);

// Configures Timer0 for the task it will perform
void initTimer0(void);

// Checks if button 1 is pressed
int isButton1Pressed(void);

// Checks if button 2 is pressed
int isButton2Pressed(void);


// Interrupt service routine to toggle pin D6
ISR(TIMER0_COMPA_vect)
{
	PORTD ^= (1 << 6);
}

// Initializes all pins which will be used
void initPorts(void)
{
	DDRD  &= (~(1 << 3)) & (~(1 << 2));
	PORTB &= (~(1 << 3)) & (~(1 << 2));

	DDRD   |= (1 << 6);
	PORTD  &= (~(1 << 6));
}

// Configures Timer0 for the task it will perform
void initTimer0(void)
{
	TCCR0A |= (1 << 1);
	TCCR0B |= (1 << 2);
	OCR0A  = 118;
	TIMSK0 |= (1 << 1);
}

// Checks if button 1 is pressed
int isButton1Pressed(void)
{
	return ((PIND >> 2) & 1);
}

// Checks if button 2 is pressed
int isButton2Pressed(void)
{
	return ((PIND >> 3) & 1);
}



int main(void)
{
	// Initializes all pins which will be used
	initPorts();

	// Configures Timer0 for the task it will perform
	initTimer0();

	// Global enable for interrupts
	sei();

	while(1)
	{
		if(!isButton1Pressed() && !isButton2Pressed())
		{
			// Prescaler as 1024
			TCCR0B |= (1 << 2) | (1 << 0);
			TCCR0B &= (~(1 << 1));

			// Calculated initial value for 261.63Hz square waves
			OCR0A  = 29;

			// Tracks on it state while the releasing lasts
			while(!isButton1Pressed() && !isButton2Pressed());
		}
		else
		{
			if(isButton1Pressed() && !isButton2Pressed())
			{
				// Prescaler as 1024
				TCCR0B |= (1 << 2) | (1 << 0);
				TCCR0B &= (~(1 << 1));

				// Calculated initial value for 329.63Hz square waves
				OCR0A  = 19;

				// Tracks on it state while the releasing lasts
				while(isButton1Pressed() && !isButton2Pressed());
			}
			else
			{
				if(!isButton1Pressed() && isButton2Pressed())
				{
					// Prescaler as 256
					TCCR0B |= (1 << 2);
					TCCR0B &= (~(1 << 1)) & (~(1 << 0));

					// Calculated initial value for 392.00Hz square waves
					OCR0A  = 94;

					// Tracks on it state while the releasing lasts
					while(!isButton1Pressed() && isButton2Pressed());
				}
				else
				{
					// Prescaler as 256
					TCCR0B |= (1 << 2) | (1 << 0);
					TCCR0B &= (~(1 << 1));

					// Calculated initial value for 523.25Hz square waves
					OCR0A  = 14;

					// Tracks on it state while the releasing lasts
					while(isButton1Pressed() && isButton2Pressed());
				}
			}
		}
	}
}

