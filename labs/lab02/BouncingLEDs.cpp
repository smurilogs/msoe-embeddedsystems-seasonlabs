/* BouncingLEDs.cpp
 *
 *      Created on: Sep 11, 2014
 *      Author: gonzagasilvas
 *      Description:
 *      makes use of the GPIO of the microcontroller used (ATmega328P) to interface five
 *      digital outputs (5 LEDs) and two digital inputs (2 switches/pushbuttons). The
 *      idea is to use the buttons to control a row of LEDs arranged in an upside down
 *	    "V"	formation. In this formation, only one of the LEDs are on at the same time,
 *      with adjacent sequential transitions of LED's states every second. The first
 *      switch can be pressed to toggle the direction of these	 transitions, which can
 *      turn from clockwise to counter clockwise, or vice versa. The second switch keeps
 *      all LEDs off as long as it is pressed.
 *
 * (PORTB0 - Switch 1 - changes rotation direction)
 * (PORTD7 - Switch 2 - turns off all LEDs)
 *
 * (PORTB1 - LED 1)
 * (PORTB2 - LED 2)
 * (PORTB3 - LED 3)
 * (PORTB4 - LED 4)
 * (PORTB5 - LED 5)
 */

// Includes
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

// Function prototypes
void initPorts(void);           // Initializes PORTS that will be used
uint8_t isSwitch1Pressed(void); // Checks if switch 1 is pressed or not
uint8_t isSwitch2Pressed(void); // Checks if switch 2 is pressed or not

int main(void)
{	
	uint8_t lastState;       // To save the last step of the LEDs transitions
	uint8_t cw = 0b11111111; // To flag transitions as Clockwise or Counter Clockwise
	initPorts();             // Calls ports initialization

	while(1)
	{
              // Checks if Switch 2 is pressed.
		if(isSwitch2Pressed())
		{
			lastState = PORTB;         // Save the last state of the rotation
			PORTB &= 0b11000001;       // Turns off all LEDs
			while(isSwitch2Pressed()); // Keep all off while Switch 2 is pressed
			PORTB = lastState; // Recovers last state when Switch 2 is released
		}


		// Checks if Switch 1 is pressed
		if(isSwitch1Pressed())
		{
			cw = ~cw;         // Toggle sequence direction
		}

		// Switches the direction transitions of LEDs, if Clockwise
	    if(cw == 0b11111111)
	    {
		     // verifies if the 5th LED is on - sequence needs to be reinitialized
	         if(PORTB == 0b00100000)
	         {
	        	 PORTB = 0b00000010; // Reinitialize the sequence.
             }
             else
             {
            	 // Makes Clockwise sequence go to the next state
		    	 PORTB = (PORTB<<1);
             }

             // If Counter Clockwise...
		}else
		{
			// Verifies if 1st LED is on - sequence needs to be reinitialized
			// Also prevent system fail if it�s initialized with switch1 pressed
			if(PORTB == 0b00000010 || PORTB == 0b00000000)
			{
				PORTB = 0b00100000; // Reinitialize the sequence
			}
			else
			{
            	// Makes Counter Clockwise sequence go to the next state
				PORTB = (PORTB>>1);
			}
		}
		// Delay for each step of any sequence.
	    _delay_ms(100);
	}
}

// Initializes PORTS that will be used.
void initPorts(void)
{
    DDRB |= 0b00111110;  // Define PORTB5 to PORTB1 as output
	DDRB &= 0b11111110;  // Define PORTB0 as input
	DDRD &= 0b01111111;  // Define PORTD7 as input
	PORTB = 0b00000001;  // Initialization of LED's state
}

// Checks if switch 1 is pressed or not.
uint8_t isSwitch1Pressed(void)
{
	return (PINB & (1<<PINB0));
}

// Checks if switch 2 is pressed or not.
uint8_t isSwitch2Pressed(void)
{
	return (PIND>>PIND7);
}
