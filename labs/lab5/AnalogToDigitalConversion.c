/*
 * AnalogToDigitalConversion.c
 *
 *  Created on: Oct 2, 2014
 *      Author: gonzagasilvas
 *      Description: The system described in this code uses the ATmega328P microcontroller analog-to-
 *                   digital converter module to measure the intensity of warm presented by a symbolic
 *                   sensor of temperature (whose variations are emulated by the output voltage of a 10
 *                   KΩ potentiometer). Using the 10-bit resolution conversion available in the ATmega328P,
 *                   it is possible to get digital values from 0 to 1023. As the requirements of this lab
 *                   presents a linear dependence between the gotten value and a range of temperature
 *                   between 0 and 100 degrees C, it was implemented a linear conversion between the two measuring
 *                   scales. After that, digital value and the temperature converted value are shown on a
 *                   LCD display in order to get the linearity visible from the equivalence of samples.
 */

// Includes
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

// Prepares ADC module for the first conversion
void initADC(void);

// Returns a value gotten from ADC module
uint16_t getADCSample(void);

// Returns the number of places needed to fit an unsigned number on LCD
uint8_t countNeededPlaces(uint16_t aNumber);

// Shows on LCD display a value in a special gap of location, considering the coordinates of the begin,
//                                                   the number of places needed and the justification
void showOnDisplay(int16_t aValue, uint8_t aColumn, uint8_t aLine, uint8_t aNPositions, uint8_t aJust);

// Turns the first decimal place of a given float value into an integer one
uint8_t getFirstDecimalPlace(float aAuxFloat);

int main(void)
{
	// Variables to be shown on LCD
	uint16_t ADCSample;
	uint16_t temperatureSample;
	uint8_t decimalPlace;

	float auxFloat;  // Auxiliary variable

	lcd_init();                 // Initializes display
	lcd_clear();                // Makes sure the display will be in blank
	lcd_home();                 // Moves the cursor to LCD first position
	lcd_print("EE2920 Room Temp"); // Writes "EE2920 Lab5A" on display’s first line

	// Draws on LCD the arrows and the symbol of Celcius degrees
    lcd_goto_xy(5,1);
    lcd_print("<->    . %cC", 0b11011111);

    initADC(); // Initializes ADC module

	while(1)
	{
		// Gets ADC sample (value from 0 to 1023)
		ADCSample = getADCSample();

		// Converts the ADC value gotten to an equivalent float value of temperature in Celcius degrees
		auxFloat = (((int)ADCSample * 100.0)/ 1023.0);

		temperatureSample = auxFloat;                  // Gets the value of temperature without decimal place
		decimalPlace = getFirstDecimalPlace(auxFloat);  // Gets to int the decimal places of the same temperature

		// Displays ADC value in a proper position on LCD
        showOnDisplay(ADCSample, 0, 1, 4, 0);

        // Displays temperature value (without decimal places) in another proper position on LCD
        showOnDisplay(temperatureSample, 9, 1, 3, 0);

        // Displays the temperature decimal places in another proper position on LCD
        showOnDisplay(decimalPlace, 13, 1, 1, 0);

		_delay_ms(300);
	}
}

// Prepares ADC module for the first conversion
void initADC(void)
{
    ADMUX  |= (1 << REFS0);                                // Sets AVcc as the reference of the system
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Sets 128 divider factor prescaler
    ADCSRA |= (1 << ADEN);                                 // Enables the ADC
}

// Returns a value gotten from ADC module
uint16_t getADCSample(void)
{
	uint16_t sample;

    ADCSRA |= (1 << ADSC);           // Starts ADC conversion
    while(ADCSRA & (1 << ADSC));     // Waits until conversion is done

    // Fits ADCH and ADCL in a single variable of 16 bits
    sample = ADCL;
    sample = (ADCH << 8) + sample;

    return sample;  // Return the gotten value
}

// Returns the number of places needed to fit an unsigned number on LCD
uint8_t countNeededPlaces(uint16_t aNumber)
{
	uint8_t neededPlaces = 1;  // To keep the number of places to be returned
	uint16_t reference = 10;   // To keep a reference value that is used in the manipulations

	// Structure to get the number of places needed to show the given number
	while(aNumber >= reference)
	{
		reference = reference * 10;
		neededPlaces++;
	}
	return neededPlaces; // Returns the number of places needed
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

// Turns the first decimal place of a given float value into an integer one
uint8_t getFirstDecimalPlace(float aAuxFloat)
{
	// Process to the transformation

	int auxInt;
	uint8_t decimalPlace;

	auxInt = ((int)(aAuxFloat * 100.0)) % 100;
    decimalPlace = ((auxInt % 100) - (auxInt % 10))/10;

    if((auxInt % 10) > 5)
    {
    	decimalPlace++;
    	if(decimalPlace == 10) decimalPlace = 9;
    }

    return decimalPlace;
}
