
/*
 * InfraredObjectSensing.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: gonzagasilvas
 */

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd/lcd.c"

#define LIMIT1 200
#define LIMIT2 100

uint8_t burst;        // States if burst must be activated or not
uint8_t valueCounter; // Gets the value of TIMER2 to distance verification

// Refreshes the LCD display according to how close the detection is
void refreshLCDDisplay(void);

// Verifies if LED needs to be activated or disactivated
void checkLEDActivation(void);

// Initializes pins that need to be initialized
void initPORTS(void);

// Initializes the routine that generates 38 kHz for burst
void initTIMER0(void);

// Initializes the routine that generates pulses of bursts
void initTIMER1(void);

// Initializes pin change interruption on pin B2
void initPINCHANGE(void);

int main(void)
{
	burst = 0;   // Initializes without burst

	// Initializes pins that need to be initialized
	initPORTS();

	// Initializes the routine that generates 38 kHz for burst
	initTIMER0();

	// Initializes the routine that generates pulses of bursts
	initTIMER1();

	// Initializes pin change interruption on pin B2
	initPINCHANGE();

	// Global enable for interrupts
	sei();

	// Initializes LCD and print fixed words on it
	lcd_init();
	lcd_clear();
	lcd_home();
	lcd_print("EE2920 Lab8A");
	lcd_goto_xy(0,1);
	lcd_print("Object = ");

	while(1)
	{
		refreshLCDDisplay();
		_delay_ms(500);
	}
}


// Refreshes the LCD display according to how close the detection is
void refreshLCDDisplay(void)
{
	if(valueCounter > LIMIT1)
	{
		lcd_goto_xy(9,1);
		lcd_print("    ");
		lcd_goto_xy(9,1);
		lcd_print("Near");
		while(valueCounter > LIMIT1);
	}
	else
	{
		if(valueCounter > LIMIT2)
		{
			lcd_goto_xy(9,1);
			lcd_print("    ");
			lcd_goto_xy(9,1);
			lcd_print("Mid");
			while(valueCounter < LIMIT1 && valueCounter > LIMIT2);
		}
		else
		{
			lcd_goto_xy(9,1);
			lcd_print("    ");
			lcd_goto_xy(9,1);
			lcd_print("Far");
			while(valueCounter < LIMIT2);
		}
	}
}

// Verifies if LED needs to be activated or disactivated
void checkLEDActivation(void)
{
	if(((PINB >> 2) & 1) == 0) PORTB |= (1 << 3);
					      else PORTB &= ~(1 << 3);
}

// Initializes pins that need to be initialized
void initPORTS(void)
{
	DDRB  |= (1 << 3) | (1 << 1);  	 // Sets pins B3 and B1 as output
	PORTB &= ~(1 << 1);              // Sets pin B1 as LOW
}

// Initializes the routine that generates 38 kHz for burst
void initTIMER0(void)
{
	TCCR0A |= (1 << 1);            // Sets CTC mode (event when OCR0A overflows)
	TCCR0B |= (1 << 2) | (1 << 0); // Sets prescaler of TIMER0 to 1024
	TCCR0B &= ~(1 << 1);
	OCR0A   = 77;                  // Initialization of OCR0A
	TIMSK0 |= (1 << 1);            // Enables interruption on each TIMER0 event
}

// Initializes the routine that generates pulses of bursts
void initTIMER1(void)
{
	TCCR1A |= (1 << 6);            // Sets toggle when OC1A match
	TCCR1B |= (1 << 3) | (1 << 1); // Sets CTC mode and prescaler of TIMER1 to 8
	OCR1A = 25;					   // Initialization of OCR1A
}

// Initializes pin change interruption on pin B2
void initPINCHANGE(void)
{
	PCICR  |= (1 << 0); // Sets to to enable the group of PCINT of pin B2
	PCMSK0 |= (1 << 2); // Enables interrupts from pin B2
}

ISR(PCINT0_vect)
{
	// If detected a falling edge in the pin B2...
	if(((PINB >> 2) & 1) == 0)
	{
		TCNT2 = 0;                     // Resets TCNT2 for a initiate new count
		TCCR2B |= (1 << 1) | (1 << 0); // TIMER2 starts counting using prescaler of 32
	}
	else // If detected a rising edge in the pin B2...
	{
		TCCR2B &= ~(1 << 1) & ~(1 << 0); // TIMER2 stops counting
		valueCounter = TCNT2;            // TCNT2 is registered
	}
}

ISR(TIMER0_COMPA_vect)
{
	if(burst == 0) // If burst must be desactivated...
	{
		// Sets prescaler to 1024
		TCCR0B |= (1 << 2) | (1 << 0);
		TCCR0B &= ~(1 << 1);

		OCR0A   = 77;        // Initialization of OCR0A
		TCCR1A &= ~(1 << 6); // Disconnects OC1A pin to the signal

		checkLEDActivation(); // Verifies if LED needs to be activated or disactivated
		burst = 1;            // States that burst needs to be activated for the next call
	}
	else // If burst must be activated...
	{
		// Resets counter
		TCNT1H = 0;
		TCNT1L = 0;

		// Sets prescaler of TIMER0 to 64
		TCCR0B |= (1 << 1) | (1 << 0);
		TCCR0B &= ~(1 << 2);

		OCR0A   = 104;      // Initialization of OCR0A
		TCCR1A |= (1 << 6); // Connects OC1A pin to the signal

		burst = 0; // States that burst needs to be activated for the next call
	}
}
