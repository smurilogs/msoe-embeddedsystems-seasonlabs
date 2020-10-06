# MSOE | EE2920 Embedded Systems Labs | AVR ATmega328P (C programming language)

### Lab 2: Bouncing LEDs

This lab makes use of the ATmega328P microcontroller's GPIO to interface five digital outputs (5 LEDs) and two digital inputs (2 switches/pushbuttons). The idea is to use the buttons to control a row of LEDs arranged in an upside down "V" formation. In this formation, only one of the LEDs are on at the same time, with adjacent sequential transitions of LED's states every second. The first switch can be pressed to toggle the direction of these transitions, which can turn from clockwise to counter clockwise, or vice versa. The second switch keeps all LEDs off as long as it's being pressed. 