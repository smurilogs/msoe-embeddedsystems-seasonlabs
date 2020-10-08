/*
 * DigitalLineSensorArrayAdvanced.cpp
 *
 *  Created on: Sep 27, 2014
 *      Author: gonzagasilvas
 *
 * Description:
 * The system described in this report uses ATmega328P microcontroller GPIO to interface
 * the QTR-8RC module in order to detect incoming events from its reflectance sensors
 * and measure the magnitude of reflection detected by them. The activity proposed
 * also makes use of a LCD display to show the state of each one of the four used
 * sensors, showing a sequence of four places on display relative to their states. The
 * system described in this report uses the ATmega328P microcontroller GPIO to interface
 * the QTR-8RC module in order to detect incoming events from its reflectance sensors
 * and measure the magnitude of reflection detected by them. The activity proposed
 * also makes use of a LCD display to show the state of each one of the four used
 * sensors, showing a sequence of four places on display relative to their states. The
 * measuring is made by counting the time until the discharge of each sensor gets the
 * threshold of the microcontroller used. If the system detects a higher reflectance,
 * compared to a reference value, the time decreases and �1� will be place in the
 * position relative to the sensor used. On the other hand, if the level of reflectance
 * gets lower, the time of discharge increases and gets higher than reference value,
 * it will be shown �0�.
 *
 */


// Includes
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

// Value of reference to determine the limit between �dark� and �light
#define REFERENCE_VALUE 50

// Counts the time until the pin indexed in PORTB gets LOW as input value
uint16_t readSensor(uint8_t pinIndex);

// Presents, in a specific LCD position '0' (�light�) or '1' (�dark�)
void printResult(uint8_t position, uint16_t counter);

int main(void)
{
	uint16_t counter;
	lcd_init();                 // Initializes display
	lcd_clear();                // Makes sure the display will be in blank
	lcd_home();                 // Moves the cursor to LCD first position
	lcd_print("EE2920 Lab4A"); // Writes "EE2920 Lab4A" on display�s first line

	// Main loop (infinite)
	while(1)
	{
		// Goes over four index, each one relative to a pin of PORTB
		for(uint8_t i = 0; i <= 3; i++)
		{
			// Gets the time of discharge until it gets the threshold
			counter = readSensor(i);

                    // Presents '1' (dark) or '0'(light) based on a value of time, in
			printResult(i, counter);          // a specific place on display,
		}

		// Delay to refresh the LCD display
		_delay_ms(500);
	}
}

// Counts the time until the pin indexed in PORTB gets LOW as input value
uint16_t readSensor(uint8_t pinIndex)
{
	uint16_t counter = 0;

	// Procedures to charge the capacitor connected to indexed PORTB pin
	DDRB  |= (0b00000001 << pinIndex);      // Indexed PORTB pin as output
	PORTB |= (0b00000001 << pinIndex);      // Indexed PORTB pin to HIGH

	_delay_us(200); // Time to ensure the charge is complete

	DDRB  &= ~(0b00000001 << pinIndex);     // Indexed PORTB pin as input
	PORTB &= ~(0b00000001 << pinIndex);     // Indexed PORTB pin pull-up OFF

	// Registers how many 10us until the discharge voltage gets the threshold
	while((PINB & (0b00000001 << pinIndex)))
	{
		_delay_us(10);
		counter++;
	}

	return counter;
}

// Presents, in a specific LCD position '0' (�light�) or '1' (�dark�)
void printResult(uint8_t position, uint16_t counter)
{
	// Sets the LCD cursor in a specific place of LCD second line
	lcd_goto_xy(position,1);

	// Establishes a value limit to define what is considered 'dark' or 'light'
	if(counter >= REFERENCE_VALUE) lcd_print("1");
	                          else lcd_print("0");
}
