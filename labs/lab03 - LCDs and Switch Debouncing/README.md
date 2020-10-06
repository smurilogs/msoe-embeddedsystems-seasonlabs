# MSOE | EE2920 Embedded Systems Labs | AVR ATmega328P (C programming language)

### Lab 3: LCD and switch de-bounce

This lab it was introduced the use of alphanumeric LCD displays. The interface between the LCD display and the ATmega328p could be easily implemented by the use of a pre-defined structure and relatively high-level functions described in include files recommended to be used in this lab.

In the basic assignment, it is shown a simple system that was able to receive the signals of buttons (1 and 2) to get their states and activate events from them. Basically, the LCD display shows the state of button one, presenting ‘1’ to button pressed and ‘0’ to not pressed. At the same time, button two is used to blink a character in the display when it is held pressed. 

The purpose of the advanced assignment of this project is to make use of a LCD display to show a counter that can be incremented (if button one is pressed) or decremented (if button one is pressed while button two is held pressed). To make better use of these buttons, it was implemented a system that uses delays and loops to avoid the detection of bouncing from mechanical switches that are being used in this assignment, and also, prevent that multiple events were detected when buttons are pressed.