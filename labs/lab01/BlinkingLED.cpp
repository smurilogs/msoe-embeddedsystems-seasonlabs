/*
 * BlinkingLED.cpp
 *
 *  Created on: Sep 11, 2014
 *      Author: smurilogs
 */

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << PORTB5);
	PORTB = 0;

	while(1)
	{
		_delay_ms(1000);
		PORTB ^= (1 << PORTB5);
	}
}

