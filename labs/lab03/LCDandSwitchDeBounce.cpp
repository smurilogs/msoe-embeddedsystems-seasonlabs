/*
 * LCDandSwitchDeBounce.cpp
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
uint8_t countNeededPlaces(int16_t aNumber); // Counts how many places a given number needs to be displayed
uint8_t isButton1Pressed(void);             // States if button 1 is pressed or not
uint8_t isButton2Pressed(void);             // States if button 2 is pressed or not
void debounceButton1(void);                 // Uses delays to debounce and prevents multiple events when the button is held
void initPorts(void);                       // Initializes PORTS that will be used
void refreshDisplay(int16_t aCounter);      // Refreshes the display, showing a selected number

int main(void)
{
	int16_t counter;            // To save the number the current number in the counter

	lcd_init();                 // Initializes display
	lcd_clear();                // Makes sure the display will be in blank
	lcd_home();                 // Moves the cursor to the first position of the display
	lcd_print("EE2920 Lab3A"); // Writes "EE2920 Lab3A" in the first line of the display
	lcd_goto_xy(0,1);           // Moves the cursor to the second line of the display
	lcd_print("Counter = ");   // Writes "Counter = ", letting 6 blank spaces until the end of the line

	initPorts();                // Initializes PORTS that will be used
	counter = 0;                // Initializes the counter with zero
	refreshDisplay(counter);    // Refresh the counter number shown in the display at first

	// Main loop (infinite)
	while(1)
	{
		// While button 2 is held pressed...
		while(isButton2Pressed())
		{
			// If the button 1 is pressed...
			if(isButton1Pressed())
			{
				counter--;               // Subtracts one from the counter
				refreshDisplay(counter); // Refresh the counter number shown in the display
				debounceButton1();       // Debounce signal from button 1 and avoid multiple events when it's held
			}
		}

		// If the button 1 is pressed...
		if(isButton1Pressed())
		{
			counter++;               // Sums one to the counter
			refreshDisplay(counter); // Refresh the counter number shown in the display
			debounceButton1();       // Debounce signal from button 1 and avoid multiple events when it's held
		}
	}
}

// Counts how many places a given number needs to be displayed
uint8_t countNeededPlaces(int16_t aNumber)
{
	uint8_t neededPlaces = 1; // To keep the number of places to be returned
	uint8_t reference = 10;   // To keep a reference value that is used in the manipulations

	// If the given number is negative...
	if(aNumber < 0)
	{
		// Gets the module of the number in order to apply the same techniques to positive and negative numbers
		aNumber = aNumber * (-1);

		// Adds one to the number of places, since the signal '-' needs to be shown
		neededPlaces++;
	}

	// Structure to get the number of places needed to show the given number
	while(aNumber >= reference)
	{
		reference = reference * 10;
		neededPlaces++;
	}

	// Returns the number of places needed
	return neededPlaces;
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

// Uses delays to debounce and prevents multiple events when the button is held
void debounceButton1(void)
{
	_delay_ms(150);             // Waits until a low-high signals stabilizes
	while(isButton1Pressed()); // Prevents multiple events when button 1 is pressed
	_delay_ms(150);			   // Waits until a high-low signals stabilizes
}

// Initializes PORTS that will be used
void initPorts(void)
{
	DDRD   &= 0b11110011; // Sets D2 and D3 as input pins
	PORTD  &= 0b11110011; // Turns off the pull-up to D2 and D3 pins
}

// Refreshes the display, showing a selected number
void refreshDisplay(int16_t aCounter)
{
	int8_t neededPlaces;
	int8_t i;

	neededPlaces = countNeededPlaces(aCounter);
	lcd_goto_xy(10,1);
	lcd_print("%d", aCounter);

	i = 10 + neededPlaces;
	for(i = i; i != 16; i++)
	{
		lcd_goto_xy(i,1);
		lcd_print(" ");
	}
}
