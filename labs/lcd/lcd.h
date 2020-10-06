
#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

#define LCD_DATA_PORT PORTD
#define LCD_DATA_PINS DDRD
#define LCD_DATA_D7 PD7
#define LCD_DATA_D6 PD6
#define LCD_DATA_D5 PD5
#define LCD_DATA_D4 PD4

#define LCD_CTRL_PORT PORTB
#define LCD_CTRL_PINS DDRB
#define LCD_CTRL_RS PB5
#define LCD_CTRL_EN PB4

void lcd_init (void);
void lcd_cmd(unsigned char cmnd);
void lcd_clear(void);
void lcd_print (char *str);
void lcd_goto_xy(char pos, char row);
void lcd_home(void);

#endif