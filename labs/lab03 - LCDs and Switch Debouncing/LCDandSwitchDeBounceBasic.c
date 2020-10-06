/*
 * LCDandSwitchDeBounce.c
 *
 *  Created on: Sep 18, 2014
 *      Author: gonzagasilvas
 */

// Includes
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

// Function prototypes
uint8_t isButton1Pressed(void);             // States if button 1 is pressed or not
uint8_t isButton2Pressed(void);             // States if button 2 is pressed or not
void initPorts(void);                       // Initializes PORTS that will be used

int main(void)
{
	int8_t state;            // To save the current state of button 1
	state = 0;               // Initializes the state with zero

	lcd_init();                 // Initializes display
	lcd_clear();                // Makes sure the display will be in blank
	lcd_home();                 // Moves the cursor to the first position of the display
	lcd_print("EE2920 Lab3B"); // Writes "EE2920 Lab3B" in the first line of the display
	lcd_goto_xy(15,0);          // Moves the cursor to the 16th position in the 1st line
	lcd_print("%d", state);    // Shows the state of button one for the first time
	lcd_goto_xy(0,1);           // Moves the cursor to the second line of the display
	lcd_print("Sergio");       // Writes "Sergio"

	initPorts();                // Initializes PORTS that will be used

	// Main loop (infinite)
	while(1)
	{
		// While button 2 is held pressed...
		while(isButton2Pressed())
		{
			// Erase the first letter of "Sergio" for 250ms
			lcd_goto_xy(0,1);
			lcd_print(" ");
			_delay_ms(250);

			// Shows the first letter of "Sergio" for 250ms
			lcd_goto_xy(0,1);
			lcd_print("S");
			_delay_ms(250);
		}

		// If button button 1 is pressed...
		if(isButton1Pressed())
		{
			// Changes the state of button 1 to '1'
			state = 1;
			lcd_goto_xy(15,0);
			lcd_print("%d", state);

			// Keep this state until button 1 is released
			while(isButton1Pressed());
		}

		// Checks if state is one whenit shouldn't
		if(state == 1)
		{
			// Forces that the state to go back to '0'
			state = 0;
			lcd_goto_xy(15,0);
			lcd_print("%d", state);
		}
	}
}

// States if button 1 is pressed or not
uint8_t isButton1Pressed(void)
{
	return ((PIND>>2) & 0b00000001);
}

// States if button 2 is pressed or not
uint8_t isButton2Pressed(void)
{
	return ((PIND>>3) & 0b00000001);
}

// Initializes PORTS that will be used
void initPorts(void)
{
	DDRD   &= 0b11110011; // Sets D2 and D3 as input pins
	PORTD  &= 0b11110011; // Turns off the pull-up to D2 and D3 pins
}
