
/*
 * CraneControlSystemBasic.cpp
 *
 *  Created on: Nov 4, 2014
 *      Author: gonzagasilvas
 *      Description: This code is the implementation of a representative system for controlling a crane.
 *      			 It is able to get 10-bit ADC samples on periods of 10 milliseconds to control a the
 *      			 duty cycle of PWM signal designed to move the position of a servo motor. The angle
 *      			 of it is proportional to the value collected on the ADC conversions, in a range of
 *      			 -45 to +45. The current angle position is also shown in a LCD display connected to
 *      			 the system.
 */

// Include files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

// Interfacing global variable
int mappedAngle;

// Initializes PORTS used
void initPORTS(void);

// Initializes TIMER0
void initTIMER0(void);

// Initializes TIMER1
void initTIMER1(void);

// Initializes ADC module
void initADC(void);

// Counts how many places a given number needs to be displayed
uint8_t countNeededPlaces(int16_t aNumber);

// Shows on LCD display a value in a special gap of location, considering the coordinates of the begin,
//                                                   the number of places needed and the justification
void showOnDisplay(int16_t aValue, uint8_t aColumn, uint8_t aLine, uint8_t aNPositions, uint8_t aJust);

int main(void)
{
	// Shows fixed characters on LCD screen
	lcd_init();
	lcd_clear();
	lcd_home();
	lcd_print("EE2920 Dsgn Proj");
	lcd_goto_xy(2,1);
	lcd_print("Angle:    %c", 0b11011111);

	// Initializes PORTS used
	initPORTS();

	// Initializes TIMER0
	initTIMER0();

	// Initializes TIMER1
	initTIMER1();

	// Initializes ADC module
	initADC();

	sei();              // Global interruption enable
	ADCSRA |= (1 << 6); // Requests the first ADC conversion
	OCR1B = 2424;       // Sets PWM for servo motor on 0 degrees

	// Infinite loop
	while(1)
	{
	    delay_ms(50);

	    // Refreshes current angle of servo motor on LCD display
	    showOnDisplay(mappedAngle, 9, 1 , 3, 0);
	}
}

// ISR activated when ADC conversion is done
ISR(ADC_vect)
{
	// Local variables
	int adc;
	int temp;
	int duty;;
	int angle;
	int mappedDuty;

	// Gets a sample from ADC module
	temp = ADCL;
    adc = (ADCH << 8) + temp;

	PORTD ^= (1 << 6);  // For debugging

    // Based on the sample, calculates the OCR1B needed for setting servo motor
	duty = (((int)adc * 1710.0)/ 1023.0);
	mappedDuty = 3279 - duty;

	// Based on the sample, calculates angle of the servo motor current position
	angle = (((int)adc * 90.0)/ 1023.0);
	mappedAngle = angle - 45;

	// Refreshes PWM in order to drive servo motor
	OCR1B = mappedDuty;
}

// ISR activated by TIMER0
ISR(TIMER0_COMPA_vect)
{
	// Makes sure that ADC conversion is really done
	if(((ADCSRA >> 6) & 1) == 0)
	{
		ADCSRA |= (1 << 6); // Starts a new conversion
	}
}
// Initializes PORTS used
void initPORTS(void)
{
	DDRB |= (1 << 2);
	DDRD |= (1 << 6);  // debug
}

// Initializes TIMER0
void initTIMER0(void)
{
	TCCR0A |= (1 << 1);  // Sets TIMER0 on CTC mode
	TIMSK0 |= (1 << 1);  // Enable interruptions from TIMER0

	// for 50 Hz events on CTC mode...
	OCR0A = 155;                    // Initializes OCR0A with 155
	TCCR0B |= (1 << 2) | (1 << 0);  // Sets prescaler as 256
}

// Initializes TIMER1
void initTIMER1(void)
{
	// Sets Fast PWM mode
	TCCR1A |= (1 << 1) | (1 << 0);
	TCCR1B |= (1 << 4) | (1 << 3);

	TCCR1A |= (1 << 5);  // ...
	OCR1A = 39999;       // Initializes OCR0A with 39999

	// Sets prescaler as 8
	TCCR1B &= ~(1 << 2) & ~(1 << 0);
	TCCR1B |= (1 << 1);
}

// Initializes ADC module
void initADC(void)
{
	DIDR0  |= (1 << 1);                       // Disables digital function on the PORTC0
	ADCSRA |= (1 << 2) | (1 << 1) | (1 << 0); //Sets the frequency of the ADC module to ....
	ADCSRA |= (1 << 3);                       // ...
	ADCSRA |= (1 << 7);						  // Enables ADC module
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

// Shows on LCD display a value in a special gap of location, considering the coordinates of the begin,
//                                                   the number of places needed and the justification
void showOnDisplay(int16_t aValue, uint8_t aColumn, uint8_t aLine, uint8_t aNPositions, uint8_t aJust)
{
	int8_t neededPlaces;
	int8_t end;
	int8_t i;

	if(aJust == 1) 	// If left justified...
	{
		// Process to show a left justified value

		neededPlaces = countNeededPlaces(aValue);
		lcd_goto_xy(aColumn, aLine);
		lcd_print("%d", aValue);

		end = aColumn + aNPositions;
		  i = aColumn + neededPlaces;
		for(i = i; i < end; i++)
		{
			lcd_goto_xy(i, aLine);
			lcd_print(" ");
		}
	}
	else // If right justified...
	{
		// Process to show a left justified value

		neededPlaces = countNeededPlaces(aValue);
		end = aColumn + (aNPositions - neededPlaces);
		  i = aColumn;
		for(i = i; i < end; i++)
		{
			lcd_goto_xy(i, aLine);
			lcd_print(" ");
		}
		lcd_goto_xy(end, aLine);
		lcd_print("%d", aValue);
	}
}
