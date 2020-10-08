#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * WaveformGenerator.cpp
 *
 *  Created on: Oct 19, 2014
 *      Author: gonzagasilvas
 */

// Initializes all pins which will be used
void initPorts(void);

// Configures Timer0 for the task it will perform
void initTimer0(void);

// Checks if button 1 is pressed
int isButton1Pressed(void);

// Checks if button 2 is pressed
int isButton2Pressed(void);


// Interrupt service routine from the match between TCNT0 and OCR0A
ISR(TIMER0_COMPA_vect)
{
	// Sets pin D6 as HIGH
	PORTD |= (1 << 6);
	TCNT0 = 0; // reset TCNT0 value
}

// Interrupt service routine from the match between TCNT0 and OCR0B
ISR(TIMER0_COMPB_vect)
{
	// Sets pin D6 as lOW
	PORTD &= (~(1 << 6));
}

// Initializes all pins which will be used
void initPorts(void)
{
	// Sets pins D3 and D2 as input pins for buttons
	DDRD  &= (~(1 << 3)) & (~(1 << 2));

	// Sets pin D6 as output for PWM signal
	DDRD |= (1 << 6);
}

// Configures Timer0 for the task it will perform
void initTimer0(void)
{
	// Sets an initial value for OCR0A
	OCR0A = 142;

	// Sets an initial value for OCR0B
	OCR0B = 14;

	// Sets a prescaler of 256
    TCCR0B |= (1 << 1) | (1 << 0);

    // Enable interrupts activated by the match of TCNT0 and OCR0A or OCR0B
    TIMSK0 |= (1 << 2) | (1 << 1) | (1 << 0);
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

    // Infinite loop
	while(1)
	{

		if(!isButton1Pressed() && !isButton2Pressed()) // If both buttons are released...
		{
			// Sets the PWM to a duty cycle of 20%
			OCR0B = 14;

			// Tracks on it state while the releasing lasts
			while(!isButton1Pressed() && !isButton2Pressed());
		}
		else
		{
			if(!isButton1Pressed() && isButton2Pressed()) // If only button 2 is hold pressed...
			{
				// Sets the PWM to a duty cycle of 25%
				OCR0B = 36;

				// Tracks on it state while the releasing lasts
				while(!isButton1Pressed() && isButton2Pressed());
			}
			else
			{
				if(isButton1Pressed() && !isButton2Pressed()) // If only button 1 is hold pressed...
				{
					// Sets the PWM to a duty cycle of 75%
					OCR0B = 107;

					// Tracks on it state while the releasing lasts
					while(isButton1Pressed() && !isButton2Pressed());
				}
				else // If both buttons are hold pressed...
				{
					// Sets the PWM to a duty cycle of 75%
					OCR0B = 129;

					// Tracks on it state while the releasing lasts
					while(isButton1Pressed() && isButton2Pressed());
				}
			}
		}
	}
}

