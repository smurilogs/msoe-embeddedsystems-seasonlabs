/*
 * ADConversionInterruptsAndAnalogSensors.cpp
 * 
 *
 *  Created on: Oct 14, 2014
 *      Author: gonzagasilvas
 */

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

uint16_t sampleQTR1A; // Global variable to store the current state of QTR-1A
uint16_t sampleLDR1;  // Global variable to store the current state of LDR 1
uint16_t sampleLDR2;  // Global variable to store the current state of LDR 2

// Initializes ports all ports that will be used
void initPorts(void);

// Initializes ADC based in the use of interruptions
void initADC(void);

// Refreshes the state of the 3 LEDs based on the QTR-1A response
void refreshLEDsGroup(uint16_t aSample);

// Turns an ADC sample from the LDR response in light intensity percentage
int turnLDRResponseIntoPercentage(uint16_t aSample);

// Returns the number of places needed to fit an unsigned number on LCD
uint8_t countNeededPlaces(uint16_t aNumber);

// Shows on LCD display a value in a special gap of location, considering the coordinates of the begin,
//                                                   the number of places needed and the justification
void showOnDisplay(int16_t aValue, uint8_t aColumn, uint8_t aLine, uint8_t aNPositions, uint8_t aJust);

int main(void)
{
	int aux;

	// Initialization of the global variables
	sampleQTR1A = 0;
	sampleLDR1 = 0;
	sampleLDR2 = 0;

	lcd_init();                 // Initializes display
	lcd_clear();                // Makes sure the display will be in blank
	lcd_home();                 // Moves the cursor to LCD first position

	// Displays fixed characters on LCD
	lcd_print("Left  =     %c", 0b00100101);
	lcd_goto_xy(0, 1);
	lcd_print("Right =     %c", 0b00100101);

	initPorts();  	// Initializes ports all ports that will be used
    initADC();      // Initializes ADC based in the use of interruptions
    sei();          // Enables Global to allows the use of interrupts from any source

	ADCSRA |= (1 << ADSC);		// Start first Conversion

	while(1)
	{
		// Gets and shows the percentage of light intensity in LDR 1
		aux = turnLDRResponseIntoPercentage(sampleLDR1);
		showOnDisplay(aux, 8, 0, 3, 0);

		// Gets and shows the percentage of light intensity in LDR 2
		aux = turnLDRResponseIntoPercentage(sampleLDR2);
		showOnDisplay(aux, 8, 1, 3, 0);

		// Refreshes the state of the 3 LEDs based on the QTR-1A response
		refreshLEDsGroup(sampleQTR1A);

		// Time to reinitialize the cycle
	    _delay_ms(50);
	}
}

// Interrupt Service Routine for interrupts coming from ADC conversion completion event
ISR(ADC_vect)
{
	// If the ADC channel set when interrupted is ADC2...
    if((ADMUX & 0b00000011) == 2)
	{
    	// Gets an ADC sample to the global variable "sampleQTR1A"
    	sampleQTR1A = ADCL;
        sampleQTR1A = (ADCH << 8) + sampleQTR1A;

        // Sets the ADC1 channel as the current one for the next interrupt
        ADMUX &= (~(1 << MUX1));
        ADMUX |= (1 << MUX0);
    }
    else
    {
    	// If the ADC channel set when interrupted is ADC1...
    	if((ADMUX & 0b00000011) == 1)
    	{
        	// Gets an ADC sample to the global variable "sampleLDR1"
    		sampleLDR1 = ADCL;
            sampleLDR1 = (ADCH << 8) + sampleLDR1;

            // Sets the ADC0 channel as the current one for the next interrupt
            ADMUX &= (~(1 << MUX1));
            ADMUX &= (~(1 << MUX0));
    	}
    	else // If the ADC channel set when interrupted is ADC0...
    	{
    		// Gets an ADC sample to the global variable "sampleLDR1"
        	sampleLDR2 = ADCL;
            sampleLDR2 = (ADCH << 8) + sampleLDR2;

            // Sets the ADC2 channel as the current one for the next interrupt
            ADMUX &= (~(1 << MUX0));
            ADMUX |= (1 << MUX1);
    	}
    }

	ADCSRA |= (1 << ADSC);  // Start Conversion new conversion
}

// Initializes ports all ports that will be used
void initPorts(void)
{
	// Sets PORTB2:0 as output in order to interface the LEDs
	DDRB  |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);

	// Turns off the digital function of PORTC2:0 in order to interface the LDRs and the QTR-1A
	DIDR0 |= (1 << ADC2D ) | (1 << ADC1D ) | (1 << ADC0D );
}

// Initializes ADC based in the use of interruptions
void initADC(void)
{
	ADMUX  |= (1 << REFS0);                                 // Sets AVcc as the reference of the system
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   // Sets 128 divider factor prescaler
    ADMUX  |= (1 << MUX1);                                  // Chooses to begin the readings using ADC2 channel
    ADCSRA |= (1 << ADEN);                                  // Enables the ADC
    ADCSRA |= (1 << ADIE);                                  // Enables ADC interrupts
}

// Refreshes the state of the 3 LEDs based on the QTR-1A response
void refreshLEDsGroup(uint16_t aSample)
{
	if(aSample <= 40) // If the sample is lower or equal to 300...
	{
		// Turns on all LEDs
		PORTB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
	}
	else // If sample is higher than 300...
	{
		if(aSample <= 400) // If between 300 and 600...
		{
			// Turns on two first LEDs and turns off the last one
			PORTB |= (1 << PORTB1) | (1 << PORTB0);
			PORTB &= (~(1 << PORTB2));
		}
		else  // If sample is higher than 600...
		{
			if(aSample <= 800) // If between 600 and 900...
			{
				// Turns on the first LED and turns off the two last ones
				PORTB |= (1 << PORTB0);
				PORTB &= (~(1 << PORTB2)) & (~(1 << PORTB1));
			}
			else // If sample is higher than 900...
			{
			    // Turns off all LEDs
				PORTB &= (~(1 << PORTB2)) & (~(1 << PORTB1)) & (~(1 << PORTB0));
			}
		}
	}
}

// Turns an ADC sample from the LDR response in light intensity percentage
int turnLDRResponseIntoPercentage(uint16_t aSample)
{
	int result;
	float floatResult;

	// Initial values for each interval
	int value1 = 30;
	int value2 = 50;
	int value3 = 100;
	int value4 = 750;

	// Manipulations to get current values from each interval
	if(aSample > 40)
	{
		if(aSample <= 70) value1 = (aSample - 40);
		else
		{
			value1 = 0;
    		if(aSample <= 120) value2 = (aSample - 70);
			else
			{
				value2 = 0;
				if(aSample <= 220) value3 = (aSample - 120);
				else
				{
					value3 = 0;
					if(aSample <= 970) value4 = (aSample - 220);
					else
					{
						value4 = 0;
					}
				}
			}
		}
	}

	// Manipulations to weight each interval according with the plot characterization
	floatResult = (float)1.0*value1 + (float)2.0*value2 + (float)11.0*value3 + (float)86.0*value4;
	floatResult = (floatResult*100)/65730.0;

	result = (int)floatResult;
	if(result == 0 || result > 100)
	{
		if(value1 == 0) result = 100;
		else result = 0;
	}
	if((result == 99 || result == 98) && value3 != 0) result = 0;

	// Returns a value from 0 to 100 to represent the light intensity from the LDR structures
	return result;
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
