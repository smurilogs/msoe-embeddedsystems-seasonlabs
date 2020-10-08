
/*
 * CraneControlSystemAdvanced.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: gonzagasilvas
 *      Description: This code is the implementation of a representative system for controlling a crane.
 *      		     It is able to decode the data sent by a remote control using IR data signals with
 *                   SONY SIRC protocol and use the decoded data to change the angle of a servo motor.
 *                   Also the button pressed function as well as the decoded binary is shown on the screen
 *                   of a LCD display
 */

// Include files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

// Interfacing global variables
int timeSample;
int startBitCaught = 0;
int index = 0;
uint16_t bitSequence;
uint8_t data = -1;
uint16_t bitSequenceShadow;
uint8_t dataShadow = -1;
int debauncing = 0;

// Initializes PORTS used
void initPORTS(void);

// Initializes TIMER1
void initTIMER1(void);

// Initializes the use of INT0 pin
void initINT0(void);

// Refreshes servo motor position based on OCR1B value
void refreshServoMotor(void);

// Prints binary number of last decoded value on LCD display
void decodeBinary(void);

// Prints button pressed description on LCD display
void decodeButton(void);

int main(void)
{
	lcd_init();
	lcd_clear();
	lcd_home();

	lcd_goto_xy(0,0);
    lcd_print(" > ");
	lcd_goto_xy(0,1);
    lcd_print(" > ");

    // Initializes PORTS used
	initPORTS();

	// Initializes TIMER1
	initTIMER1();

	// Initializes the use of INT0 pin
	initINT0();

	sei();         // Global interruption enable
	OCR1B = 2424;  // Sets PWM for servo motor on 0 degrees

	// Infinite loop
	while(1)
	{
	    delay_ms(100);

	    dataShadow = data;
	    bitSequenceShadow = bitSequence;

	    refreshServoMotor();
	    decodeButton();
	    decodeBinary();
	}
}

// ISR activated when any edge is identified on INT0 pin
ISR(INT0_vect)
{
	if(((PIND >> 2) & 1) == 0)  // If that is an falling edge...
	{
		TCCR2B |= (1 << 2) | (1 << 1); // TIMER2 starts counting with a prescaler of 256
	}
	else  // If that is an rising edge...
	{
		TCCR2B &= ~(1 << 2) & ~(1 << 1); // TIMER2 stops counting
		timeSample = TCNT2;              // The time on the TCNT2 is saved
		TCNT2 = 0;                       // TCNT2 is set as zero for the next counts

		if(startBitCaught == 1) // If a start bit was identified...
		{
			if(30 <= timeSample && timeSample <= 48)  // If sample of time is in a interval considered a low for zero bit...
				bitSequence &= ~(1 << index);         // Sets Indexed bit as zero
			else
			{
				if(68 <= timeSample && timeSample <= 84)  // If sample of time is in a interval considered a low for one bit...
					bitSequence |= (1 << index);          // Sets Indexed bit as zero
			}

			index++;        // Step the indexed bit to the next position
			if(index == 13) // If index presents a bit outside of the number of bits on SONY protocol...
			{
				startBitCaught == 0; // Sets start bit identification flag as zero for the next words
				timeSample = 0;      // Sets time sample as zero
				index = 0;			 // Resets index to zero
				data = bitSequence & ~(1 << 7); // Gets only the bits of data to the global variable "data"
			}
		}
		else // If it did not identify a start bit on this cycle...
		{
			debauncing = 0;                            // Resets debouncing value
			if(140 <= timeSample && timeSample <= 160) // If sample of time is in a interval considered a low for a start bit...
				startBitCaught = 1;                    // Sets start bit identification flag as one to start getting the bits
		}
	}
}

// Initializes PORTS used
void initPORTS(void)
{
	DDRB |= (1 << 2);
}

// Initializes TIMER1
void initTIMER1(void)
{
	// Sets Fast PWM mode
	TCCR1A |= (1 << 1) | (1 << 0);
	TCCR1B |= (1 << 4) | (1 << 3);

	TCCR1A |= (1 << 5);
	OCR1A = 39999;

	// Sets prescaler as 8
	TCCR1B &= ~(1 << 2) & ~(1 << 0);
	TCCR1B |= (1 << 1);
}

// Initializes the use of INT0 pin
void initINT0(void)
{
	EICRA |= (1 << 0);  // Configures for interrupt on any edge on INT0 pin
	EIMSK |= (1 << 0);  // Enables interrupts from INT0 pin
}

// Refreshes servo motor position based on OCR1B value
void refreshServoMotor(void)
{
	if(dataShadow == 3) OCR1B = 3279;
	if(dataShadow == 4) OCR1B = 2424;
	if(dataShadow == 5) OCR1B = 1569;

	if(dataShadow == 18 && OCR1B > 1664)
	{
		debauncing++;
		if(debauncing == 2)
		{
			OCR1B = OCR1B - 95;
			debauncing = 0;
		}
	}

	if(dataShadow == 19 && OCR1B < 3184)
	{
		debauncing++;
		if(debauncing == 2)
		{
			OCR1B = OCR1B + 95;
			debauncing = 0;
		}
	}
	data = -1;
}

// Prints binary number of last decoded value on LCD display
void decodeBinary(void)
{
	int i, n;

	for(i = 0; i < 12 ; i++)
	{
		if(((bitSequenceShadow >> i) & 1) == 0)
			n = 0;
		else
			n = 1;

		lcd_goto_xy(14 - i, 1);
		lcd_print("%d", n);
	}
}

// Prints button pressed description on LCD display
void decodeButton(void)
{
    lcd_goto_xy(3,0);
    switch(dataShadow)
    {
    	case 0:
		    	lcd_print("Key 1    ");
    			break;
    	case 1:
    			lcd_print("Key 2    ");
    			break;
    	case 2:
    			lcd_print("Key 3    ");
    			break;
    	case 3:
    			lcd_print("Key 4    ");
    			break;
    	case 4:
    			lcd_print("Key 5    ");
    			break;
    	case 5:
    			lcd_print("Key 6    ");
    			break;
    	case 6:
    			lcd_print("Key 7    ");
    			break;
    	case 7:
    			lcd_print("Key 8    ");
    			break;
    	case 8:
    			lcd_print("Key 9    ");
    			break;
    	case 9:
		    	lcd_print("Key 0    ");
    			break;
    	case 16:
    			lcd_print("Channel +");
    			break;
    	case 17:
    			lcd_print("Channel -");
    			break;
    	case 18:
    			lcd_print("Volume + ");
    			break;
    	case 19:
    			lcd_print("Volume - ");
    			break;
    	case 20:
    			lcd_print("Mute     ");
    			break;
    	case 21:
    			lcd_print("Power    ");
    			break;
    	case 11:
    			lcd_print("Enter    ");
    			break;
    	case 37:
    			lcd_print("Input    ");
    			break;
    	case 116:
    			lcd_print("UP       ");
    			break;
    	case 117:
    			lcd_print("DOWN     ");
    			break;
    	case 51:
    			lcd_print("RIGHT    ");
    			break;
    	case 52:
    			lcd_print("LEFT     ");
    			break;
    	default:
    			lcd_print("         ");
    			data = -1;
    }
}
